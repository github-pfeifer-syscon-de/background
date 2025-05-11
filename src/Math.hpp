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

#pragma once

#include <cmath>

// decorator, astronomic unit (distance earth to sun)
consteval double operator ""_AU(const long double dist)
{
    return static_cast<double>(dist);
}

// decorator unit for angle ° \u00ba not going to work c++ 14 see makefile as well
//consteval double operator ""_°(long double dist)
//{
//    return static_cast<double>(dist);
//}


// these are remnants of porting from java...
class Math
{
public:
    Math();
    explicit Math(const Math& orig) = delete;
    virtual ~Math() = default;
    // can be replaced by std::numers<pi> with c++20
    static constexpr auto PI{3.141592653589793238462643383279502884};
    static constexpr auto DEGREEE2RADIANS{PI / 180.0};
    static constexpr auto HOURS2RADIANS{PI / 12.0};
    static constexpr auto HALF_PI{PI * 0.5};
    static constexpr auto TWO_PI{PI * 2.0};
    static inline double toRadians(double deg)
    {
        return deg * DEGREEE2RADIANS;
    }
    static inline double toDegrees(double rad)
    {
        return rad / DEGREEE2RADIANS;
    }
    static inline double toRadianHours(double hours)
    {
        return hours * HOURS2RADIANS;
    }
    static inline double toHoursRadian(double rad)
    {
        return rad / HOURS2RADIANS;
    }
    template <typename T>
    static inline T mix(T x, T y, T a, bool limit = true)
    {
        auto r = x*(1-a)+y*a;
        if (limit) {
            r = std::max(r, std::min(x, y));
            r = std::min(r, std::max(x, y));
        }
        return r;
    }
private:

};

class Angle
{
public:
    Angle();
    Angle(double _radians);
    virtual ~Angle() = default;

    double toDegrees();
    static Angle fromDegrees(double degrees);

private:
    double radians;

};

