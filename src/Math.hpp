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

#include <iostream>
#include <cmath>
#include <numbers>
#include <vector>
#include <exception>
#include <format>

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

template<typename T, T... args>
struct LinGradient {
    const std::array<T, sizeof...(args)> m_stops{args...};
    static_assert(sizeof...(args) > 0 && sizeof...(args) % 2 == 0, "expecting a even number of values to be used as pairs");
    template<typename U>
    static constexpr bool is_ascending(U prev) {
        return true;
    }
    template<typename U, typename...rem>
    static constexpr bool is_ascending(U prev, U arg, rem...argse) {
        if (sizeof...(argse) % 2 == 1) {
            if (arg - prev <= 0.0
             || arg < 0.0
             || arg > 1.0) {
                return false;
            }
            return is_ascending(arg, argse...);
        }
        return is_ascending(prev, argse...);
    }
    static_assert(is_ascending(-0.0000001, args...), "expecting ascending stop source values in range 0..1");
    T intrapolate(T value)
    {
        for (size_t i = 0; i < m_stops.size(); i+=2) {
            if (m_stops[i] > value) {
                if (i == 0) {       // return first entry
                    return m_stops[1];
                }
                T diff = (m_stops[i] - m_stops[i-2]);
                //if (diff <= 0.0) {  // ignore entries with zero distance ... avoid div by zero
                //    std::cout << std::format("expecting ascending stop source values, but {}[{}] and {}[{}] match -> ignoring",
                //        m_stops[i-2], i-2, m_stops[i], i) << std::endl;
                //    continue;
                //}
                T step = (value - m_stops[i-2]) / diff;
                return std::lerp(m_stops[i-1], m_stops[i+1], step);
            }
        }
        return m_stops[m_stops.size() - 1];
    }
};

// this might still have some java smell
class Math
{
public:
    Math();
    explicit Math(const Math& orig) = delete;
    virtual ~Math() = default;

    static constexpr double PI{std::numbers::pi};
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


