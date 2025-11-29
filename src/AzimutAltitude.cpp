/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2023 RPf 
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

#include <cmath>
#include <iostream>

#include "AzimutAltitude.hpp"
#include "Math.hpp"
#include "Point2D.hpp"
#include "Layout.hpp"

AzimutAltitude::AzimutAltitude()
: AzimutAltitude(0.0, 0.0)
{
}

AzimutAltitude::AzimutAltitude(double _azimut, double _altitude)
: azimut{_azimut}
, altitude{_altitude}
{
}

bool
AzimutAltitude::isVisible()
{
    return altitude >= 0.0;	    // the altitude must be > 0 for star to be visible
}

Point2D
AzimutAltitude::toScreen(const Layout& layout) {
	// using https://astronomy.stackexchange.com/questions/35882/how-to-make-projection-from-altitude-and-azimuth-to-screen-with-screen-coordinat
	//double theta = altitude + (std::PI / 2.0); //Convert range to 0deg to 180deg
	double x = std::cos(altitude) * std::sin(azimut);
	double y = std::cos(altitude) * std::cos(azimut);
	double z = std::sin(altitude);
	double r = layout.getMin() / 2.0;
	double xm = r * (-x / (1.0 + z));   // negativ to match sellarium e<->w
	double ym = r * (-y / (1.0 + z));   // negativ to adapt to screen quadrant
    //if (std::abs(xm) > r || std::abs(ym) > r) {
    //std::cout << "az " << azimut << " alt " << altitude << " x " << x << " y " << y << " z " << z << " r " << r << " xm " << xm << " ym " << ym << "\n";
    //}
	return Point2D(xm, ym);
}

double
AzimutAltitude::getAzimut()
{
	return azimut;
}

double
AzimutAltitude::getAltitude()
{
    return altitude;
}

double
AzimutAltitude::getAzimutDegrees()
{
    return Math::toDegrees(azimut);
}

double
AzimutAltitude::getAltitudeDegrees()
{
    return Math::toDegrees(altitude);
}


