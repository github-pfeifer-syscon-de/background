/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#pragma once

#include <memory>

#include "Point2D.hpp"
#include "Layout.hpp"

/**
 * angular coordinate system, used when it is about visibility
 *   e.g. for a specific time & place on earth
 *
 */
class AzimutAltitude
{
public:
    AzimutAltitude();
    AzimutAltitude(double azimut, double altitude);
    explicit AzimutAltitude(const AzimutAltitude& orig) = delete;
    virtual ~AzimutAltitude() = default;

    bool isVisible();

    //Greg Miller (gmiller@gregmiller.net) 2022
    //Released as public domain

    //Angles must be in radians
    //Polar coordinates:
    //Theta is vertical pi/2 to -pi/2 (usually lattitude or declination)
    //Phi is horizontal 0 to 2pi, or -pi to pi (usually longitude or Right Ascension)
    //R is the radius in any units
    //
    //Rectangular:
    //x is left/right, y is forward/backward, z is up/down
    Point2D toScreen(const Layout& layout);

    double getAzimut();
    double getAltitude();
    double getAzimutDegrees();
    double getAltitudeDegrees();




private:
    double azimut;    // the values are in radians
    double altitude;

};

