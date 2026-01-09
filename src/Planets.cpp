/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf
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


#include "Planets.hpp"
#include "Math.hpp"
#include "psc_format.hpp"


Planets::Planets()
{
}

std::vector<PtrPlanet>
Planets::getOtherPlanets()
{
    if (m_planets.empty()) {
        m_planets.reserve(8);
        auto mercury = std::make_shared<Planet>("Mercury",
            Elements{
                .a{0.38709927_AU},
                .e{0.20563593},
                .I{7.00497902},
                .L{252.25032350},
                .w{77.45779628},
                .O{48.33076593}},
            Elements{
                .a{0.00000037},
                .e{0.00001906},
                .I{-0.00594749},
                .L{149472.67411175},
                .w{0.16047689},
                .O{-0.12534081}});
        m_planets.emplace_back(std::move(mercury));

        auto venus = std::make_shared<Planet>("Venus",
            Elements{
                .a{0.72333566_AU},
                .e{0.00677672},
                .I{3.39467605},
                .L{181.97909950},
                .w{131.60246718},
                .O{76.67984255}},
            Elements{
                .a{0.00000390},
                .e{-0.00004107},
                .I{-0.00078890},
                .L{58517.81538729},
                .w{0.00268329},
                .O{-0.27769418}});
        m_planets.emplace_back(std::move(venus));

        auto mars =  std::make_shared<Planet>("Mars",
            Elements{
                .a{1.52371034_AU},
                .e{0.09339410},
                .I{1.84969142},
                .L{-4.55343205},
                .w{-23.94362959},
                .O{49.55953891}},
            Elements {
                .a{0.00001847},
                .e{0.00007882},
                .I{-0.00813131},
                .L{19140.30268499},
                .w{0.44441088},
                .O{-0.29257343}});
        m_planets.emplace_back(std::move(mars));

        auto jupiter = std::make_shared<Planet>("Jupiter",
             Elements{
                .a{5.20288700_AU},
                .e{0.04838624},
                .I{1.30439695},
                .L{34.39644051},
                .w{14.72847983},
                .O{100.47390909}},
            Elements{
                .a{-0.00011607},
                .e{-0.00013253},
                .I{-0.00183714},
                .L{3034.74612775},
                .w{0.21252668},
                .O{0.20469106}});
        m_planets.emplace_back(std::move(jupiter));

        auto saturn = std::make_shared<Planet>("Saturn",
            Elements{
                .a{9.53667594_AU},
                .e{0.05386179},
                .I{2.48599187},
                .L{49.95424423},
                .w{92.59887831},
                .O{113.66242448}},
            Elements{
                .a{-0.00125060},
                .e{-0.00050991},
                .I{0.00193609},
                .L{1222.49362201},
                .w{-0.41897216},
                .O{-0.28867794}});
        m_planets.emplace_back(std::move(saturn));

        auto uranus = std::make_shared<Planet>("Uranus",
            Elements{
                .a{19.18916464_AU},
                .e{0.04725744},
                .I{0.77263783},
                .L{313.23810451},
                .w{170.95427630},
                .O{74.01692503}},
            Elements{
                .a{-0.00196176},
                .e{-0.00004397},
                .I{-0.00242939},
                .L{428.48202785},
                .w{0.40805281},
                .O{0.04240589}});
        m_planets.emplace_back(std::move(uranus));

        auto neptune = std::make_shared<Planet>("Neptune",
                Elements{
                    .a{30.06992276_AU},
                    .e{0.00859048},
                    .I{1.77004347},
                    .L{-55.12002969},
                    .w{44.96476227},
                    .O{131.78422574}},
                Elements{
                    .a{0.00026291},
                    .e{0.00005105},
                    .I{0.00035372},
                    .L{218.45945325},
                    .w{-0.32241464},
                    .O{-0.00508664}});
        m_planets.emplace_back(std::move(neptune));
    }
    return m_planets;
}

PtrPlanet
Planets::find(const char* name)
{
    auto planets = getOtherPlanets();
    for (auto& planet : planets) {
        if (planet->getName() == name) {
            return planet;
        }
    }
    throw std::invalid_argument(psc::fmt::format("Planet {} was not found!", name));
}


PtrPlanet
Planets::getEarth()
{
    if (!m_earth) {
        m_earth = std::make_shared<Planet>("Earth",
        Elements{
            .a{1.00000261_AU},
            .e{0.01671123},
            .I{-0.00001531},
            .L{100.46457166},
            .w{102.93768193},
            .O{0.0}},
        Elements{
            .a{0.00000562},
            .e{-0.00004392},
            .I{-0.01294668},
            .L{35999.37244981},
            .w{0.32327364},
            .O{0.0}});
    }
    return m_earth;
}

