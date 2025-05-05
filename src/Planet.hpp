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

#include <array>
#include <string>
#include <memory>

#include "RaDecPlanet.hpp"
#include "JulianDate.hpp"

struct Elements
{
    double a;
    double e;
    double I;
    double L;
    double w;
    double O;
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
    std::array<double,3> computePlanetPosition(const JulianDate& jd);
    std::array<double,3> posToEarth(const JulianDate& jd);
    std::shared_ptr<RaDecPlanet> rectToPolar(const std::array<double,3>& xyz);

    //https://ssd.jpl.nasa.gov/planets/approx_pos.html
    // at the moment using "short" term values (1850-2050)
    const Elements& getElements();

    const Elements& getRates();

    std::array<double,4> getExtraTerms();	// these will be used for long method
private:
    double solveKepler(double M, double e, double E);
    std::array<double,3> sub(const std::array<double,3>& xyz1, const std::array<double,3>& xyz2);

    std::string m_name;
    const Elements m_elements;
    const Elements m_rates;
    std::array<double,4> m_extraTerms;
};

using PtrPlanet = std::shared_ptr<Planet>;