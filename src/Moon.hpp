/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#pragma once

#include <cmath>
#include <memory>
#include <gtkmm.h>

#include "RaDec.hpp"
#include "JulianDate.hpp"
#include "Phase.hpp"

class Moon
{
public:

    Moon();
    explicit Moon(const Moon& orig) = delete;
    virtual ~Moon() = default;


    //Low precision geocentric moon position (RA,DEC) from Astronomical Almanac page D22 (2017 ed)
    static std::shared_ptr<RaDec> position(const JulianDate& jd);


    static Phase getPhase(const JulianDate& jd);

private:
    static double constrain(double d);
    static double sind(double r);
    static double cosd(double r);
};

