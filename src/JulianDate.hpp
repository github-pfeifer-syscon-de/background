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

#include "glibmm.h"

class JulianDate
{
public:
    explicit JulianDate(const JulianDate& orig) = delete;
    virtual ~JulianDate() = default;

    JulianDate(const Glib::DateTime& cal);
    JulianDate(double jd);
    double getJulianDate() const;
    // to epoch
    double toJulianDateE2000centuries() const;
    // to epoch 
    double toJulianDateE2000day() const;

    constexpr static auto S_PER_JULIAN_YEAR = 86400.0;
    constexpr static auto DAYS_PER_CENTURY = 36525.0;
    constexpr static auto E2000_JULIAN = 2451545.0;
    constexpr static auto JULIAN_1970_OFFS = 2440587.5;

private:
    double jd;	// jd is Julian Date in UTC (days)

};

