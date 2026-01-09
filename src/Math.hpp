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

// this might still have some java smell
class Math
{
public:
    Math();
    explicit Math(const Math& orig) = delete;
    virtual ~Math() = default;

    // why this is eliminating the conversion warning?
    template<typename T>
        using _Enable_if_floating = std::enable_if_t<std::is_floating_point_v<T>, T>;

    // can be replaced by std::numbers<pi> with c++20
    template<typename T>
        static constexpr T PI_V
            = _Enable_if_floating<T>(3.141592653589793238462643383279502884L);

    static constexpr double PI = PI_V<double>;
    static constexpr auto DEGREE2RADIANS{PI / 180.0};
    static constexpr auto HOURS2RADIANS{PI / 12.0};
    static constexpr auto HALF_PI{PI * 0.5};
    static constexpr auto TWO_PI{PI * 2.0};
    static inline double toRadians(double deg)
    {
        return deg * DEGREE2RADIANS;
    }
    static inline double toDegrees(double rad)
    {
        return rad / DEGREE2RADIANS;
    }
    static inline double toRadianHours(double hours)
    {
        return hours * HOURS2RADIANS;
    }
    static inline double toHoursRadian(double rad)
    {
        return rad / HOURS2RADIANS;
    }
    /**
     * @param x, the end of range that will be used for 0
     * @param y, the end of range that will be used for 1
     * @param a, the range from 0 to 1
     * @param limit, true result will we be in range [x..y] even if a is not in range 0..1
     * @return value in range [x..y] mapped by a in range [0..1]
     */
    template <typename T>
    static inline T mix(T x, T y, T a, bool limit = true)
    {
        auto r = x*(static_cast<T>(1)-a)+y*a;
        if (limit) {
            r = std::max(r, std::min(x, y));
            r = std::min(r, std::max(x, y));
        }
        return r;
    }
private:

};


