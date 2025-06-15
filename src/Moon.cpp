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


#include "Moon.hpp"
#include "Math.hpp"
#include "Renderer.hpp"

Moon::Moon()
{

}


double
Moon::sind(double r)
{
	return std::sin(Math::toRadians(r));
}

double
Moon::cosd(double r)
{
	return std::cos(Math::toRadians(r));
}

//Low precision geocentric moon position (RA,DEC) from Astronomical Almanac page D22 (2017 ed)
std::shared_ptr<RaDec>
Moon::position(const JulianDate& jd) {
	double T = jd.toJulianDateE2000centuries();
	double L = 218.32 + 481267.881*T + 6.29 * sind(135.0 + 477198.87 * T) - 1.27 * sind(259.3 - 413335.36 * T) + 0.66 * sind(235.7 + 890534.22 * T) + 0.21*sind(269.9 + 954397.74 * T) - 0.19*sind(357.5 + 35999.05 * T) - 0.11*sind(186.5 + 966404.03 * T);
	double B = 5.13 * sind(93.3 + 483202.02 * T) + 0.28 * sind(228.2 + 960400.89 * T) - 0.28 * sind(318.3 + 6003.15 * T) - 0.17 * sind(217.6 - 407332.21 * T);
	//double P = 0.9508 + 0.0518 * cosd(135.0 + 477198.87 * T) + 0.0095*cosd(259.3 - 413335.36 * T) + 0.0078 * cosd(235.7 + 890534.22 * T) + 0.0028 * cosd(269.9 + 954397.74 * T);
	//double SD = 0.2724 * P;
	//double r = 1.0 / sind(P);
	double l = cosd(B) * cosd(L);
	double m = 0.9175 * cosd(B) * sind(L) - 0.3978 * sind(B);
	double n = 0.3978 * cosd(B) * sind(L) + 0.9175 * sind(B);

	double ra = std::atan2(m, l);
	if (ra < 0.0){
	    ra += Math::TWO_PI;
	}
	double dec = std::asin(n);
	return std::make_shared<RaDec>(ra, dec);
}

double
Moon::constrain(double d)
{
	double t = std::fmod(d, 360.0);
	if (t < 0.0) {
	    t += 360.0;
	}
    return t;
}

Phase
Moon::getPhase(const JulianDate& jd)
{
	const double T = jd.toJulianDateE2000centuries();

	const double T2 = T * T;
	const double T3 = T2 * T;
	const double T4 = T3 * T;
	double D = Math::toRadians(constrain(297.8501921 + 445267.1114034*T - 0.0018819*T2 + 1.0/545868.0*T3 - 1.0/113065000.0*T4)); //47.2
	double M = Math::toRadians(constrain(357.5291092 + 35999.0502909*T - 0.0001536*T2 + 1.0/24490000.0*T3)); //47.3
	double Mp = Math::toRadians(constrain(134.9633964 + 477198.8675055*T + 0.0087414*T2 + 1.0/69699.0*T3 - 1.0/14712000.0*T4)); //47.4

	//48.4
	double i = Math::toRadians(constrain(180.0 - Math::toDegrees(D) - 6.289 * std::sin(Mp) + 2.1 * std::sin(M) -1.274 * std::sin(2.0*D - Mp) -0.658 * std::sin(2*D) -0.214 * std::sin(2*Mp) -0.11 * std::sin(D)));
	return Phase(i);
}


