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
#include <cmath>

#include "AzimutAltitude.hpp"
#include "RaDec.hpp"
#include "JulianDate.hpp"

class GeoPosition
{
public:
    GeoPosition();
    GeoPosition(const GeoPosition& orig) = default;
    virtual ~GeoPosition() = default;
    GeoPosition(const double& _lon, const double& _lat);

    double getLonDegrees() const;
    void setLonDegrees(double lon);
    double getLatDegrees() const;
    void setLatDegrees(double lat);
    double getLonRad() const;
    double getLatRad() const;

    //Greg Miller (gmiller@gregmiller.net) 2021
    //Released as public domain
    //http://www.celestialprogramming.com/
    std::shared_ptr<AzimutAltitude> toAzimutAltitude(const std::shared_ptr<RaDec>& raDec, const JulianDate& jd) const;


private:
    double lon{};	    // values are in degrees
    double lat{};

    double greenwichMeanSiderealTime(const JulianDate& jd) const;
    double earthRotationAngle(const JulianDate& jd) const;


};

