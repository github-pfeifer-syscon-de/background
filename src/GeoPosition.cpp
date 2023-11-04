/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2023 RPf <gpl3@pfeifer-syscon.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#include "GeoPosition.hpp"

#include "Math.hpp"

GeoPosition::GeoPosition()
{
}

GeoPosition::GeoPosition(const double& _lon, const double& _lat)
{
	lon = _lon;
	lat = _lat;
}

double
GeoPosition::getLonDegrees() const
{
    return lon;
}

void
GeoPosition::setLonDegrees(double _lon)
{
    lon = _lon;
}

double
GeoPosition::getLatDegrees() const
{
    return lat;
}

void
GeoPosition::setLatDegrees(double _lat)
{
    lat = _lat;
}

double
GeoPosition::getLonRad() const
{
    return Math::toRadians(lon);
}

double
GeoPosition::getLatRad() const
{
    return Math::toRadians(lat);
}

std::shared_ptr<AzimutAltitude>
GeoPosition::toAzimutAltitude(const std::shared_ptr<RaDec>& raDec, const JulianDate& jd) const
{
    double lonRad = getLonRad();
    double latRad = getLatRad();
    double raRad = raDec->getRaRad();
    double decRad = raDec->getDecRad();
    //Meeus 13.5 and 13.6, modified so West longitudes are negative and 0 is North
    //System.out.format("jd %.3f\n", jd);
    const double gmst = greenwichMeanSiderealTime(jd);
    //System.out.format("gmst %.3f\n", gmst);
    double localSiderealTime = std::remainder((gmst + lonRad), (M_PI2));
    //System.out.format("localSiderealTime %.3f\n", localSiderealTime);

    double H = (localSiderealTime - raRad);
    //System.out.format("H %.3f\n", H);
    if (H < 0.0) {
        H += M_PI2;
    }
    if (H > M_PI) {
        H = H - M_PI2;
    }

    double az = (std::atan2(std::sin(H), std::cos(H) * std::sin(latRad) - std::tan(decRad) * std::cos(latRad)));
    //System.out.format("az %.3f\n", az);
    double a = (std::asin(std::sin(latRad) * std::sin(decRad) + std::cos(latRad) * std::cos(decRad) * std::cos(H)));
    //System.out.format("a %.3f\n", a);
    az -= M_PI;
    if (az < 0.0) {
        az += M_PI2;
    }
    return std::make_shared<AzimutAltitude>(az, a);
}

double
GeoPosition::greenwichMeanSiderealTime(const JulianDate& jd) const
{
    //"Expressions for IAU 2000 precession quantities" N. Capitaine1,P.T.Wallace2, and J. Chapront
    const double t = jd.toJulianDateE2000centuries();
    //System.out.format("greenw. t %.3f\n", t);

    double gmst = earthRotationAngle(jd) + Math::toRadians((0.014506 + 4612.156534*t + 1.3915817*t*t - 0.00000044 *t*t*t - 0.000029956*t*t*t*t - 0.0000000368*t*t*t*t*t)/60.0/60.0);  //eq 42
    //System.out.format("gmst %.3f\n", gmst);
    gmst = std::remainder(gmst, M_PI2);
    if (gmst < 0.0) {
        gmst += M_PI2;
    }

    return gmst;
}

double
GeoPosition::earthRotationAngle(const JulianDate& jd) const
{
	//IERS Technical Note No. 32
	const double t = jd.toJulianDateE2000day();
	//System.out.format("earth t %.3f\n", t);
	const double f = std::remainder(t, 1.0);
	//System.out.format("f %.3f\n", f);

	double theta = M_PI2 * (f + 0.7790572732640 + 0.00273781191135448 * t); //eq 14
	//System.out.format("thet %.3f\n", theta);
	theta = std::remainder(theta, M_PI2);
	if (theta < 0.0) {
	    theta += M_PI2;
	}
	return theta;
}

