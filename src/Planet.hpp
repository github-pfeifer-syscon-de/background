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

#include <array>
#include <string>
#include <memory>

#include "RaDecPlanet.hpp"
#include "JulianDate.hpp"
#include "Math.hpp"

struct Elements
{
    double a;
    double e;
    double I;
    double L;
    double w;
    double O;
};

struct CarthesianCoord
{
    double x{};
    double y{};
    double z{};

    CarthesianCoord operator-(const CarthesianCoord& oth)
    {
        return CarthesianCoord {
            .x = x -  oth.x,
            .y = y - oth.y,
            .z = z - oth.z,};
    }
    std::shared_ptr<RaDecPlanet> toRaDecPlanet() const
    {
        // convert from Cartesian to polar coordinates
        const double r = std::sqrt(x * x + y * y + z * z);
        double ra = std::atan2(y, x);
        double dec = std::acos(z / r);

        // Make sure ra is positive
        if (ra < 0.0) {
            ra += Math::TWO_PI;
        }
        // Make dec is in range +/-90deg
        dec = Math::HALF_PI - dec;
        return std::make_shared<RaDecPlanet>(ra, dec, r);
    }
};

class Planet
{
public:
    Planet(const std::string& name, const Elements& elements, const Elements& rates, const std::array<double,4>& extraTerms = std::array<double,4>{});
    Planet() = default;
    explicit Planet(const Planet& orig) = delete;

    std::string getName();

    std::shared_ptr<RaDecPlanet> getRaDecPositon(const JulianDate& jd);
protected:
    CarthesianCoord computePlanetPosition(const JulianDate& jd);
    CarthesianCoord posToEarth(const JulianDate& jd);
    std::shared_ptr<RaDecPlanet> rectToPolar(const CarthesianCoord& xyz);

    //https://ssd.jpl.nasa.gov/planets/approx_pos.html
    // at the moment using "short" term values (1850-2050)
    const Elements& getElements();

    const Elements& getRates();

    std::array<double,4> getExtraTerms();	// these will be used for long method
private:
    double solveKepler(double M, double e, double E);

    std::string m_name;
    const Elements m_elements;
    const Elements m_rates;
    std::array<double,4> m_extraTerms;
};

using PtrPlanet = std::shared_ptr<Planet>;