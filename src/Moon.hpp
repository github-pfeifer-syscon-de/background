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
    // https://stackoverflow.com/questions/1734745/how-to-create-circle-with-b%c3%a9zier-curves#27863181
    // for mathematicians this is a approximation for all other people it is a circle segment
    static constexpr auto BEZIER_APPROX = (4.0 * (std::sqrt(2.0) - 1.0) / 3.0);

    Moon();
    explicit Moon(const Moon& orig) = delete;
    virtual ~Moon() = default;


    /**
     * is draw into rect of size and orgin in the top left
     */
    void showPhase(const JulianDate& jd, const Cairo::RefPtr<Cairo::Context>& cr, double radius);

    //Low precision geocentric moon position (RA,DEC) from Astronomical Almanac page D22 (2017 ed)
    static std::shared_ptr<RaDec> position(const JulianDate& jd);


    static Phase getPhase(const JulianDate& jd);

private:
    static double constrain(double d);
    static double sind(double r);
    static double cosd(double r);
    /**
     * @param radius
     * @param phase 0 right .. 1 center .. 2 left
     */
    void halfRight(const Cairo::RefPtr<Cairo::Context>& cr, double radius, double phase);
};

