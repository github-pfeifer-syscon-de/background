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

#include <iostream>

#include "Poly.hpp"

Poly::Poly()
{
}

const std::vector<std::shared_ptr<RaDec>>
Poly::getPoints()
{
    return m_points;
}

void
Poly::setIntensity(int intensity)
{
    m_intensity = intensity;
}

int
Poly::getIntensity()
{
    return m_intensity;
}

// Same as geoJson but prevent requiring additional lib
void
Poly::read(JsonArray* poly)
{
    uint32_t coordCount = json_array_get_length(poly);
    m_points.reserve(coordCount);
    //std::cout << "     " << __FILE__ << "::read coord " << coordCount << std::endl;
    std::shared_ptr<RaDec> lastRaDec;
    for (uint32_t nCoord = 0; nCoord < coordCount; ++nCoord) {
        JsonArray* coord =json_array_get_array_element(poly, nCoord);
        uint32_t coordComp = json_array_get_length(coord);
        if (coordComp == 2) {
            double ra = json_array_get_double_element(coord, 0);
            double dec = json_array_get_double_element(coord, 1);
            auto raDec = std::make_shared<RaDec>();
            raDec->setRaDegrees(ra);
            raDec->setDecDegrees(dec);
            // exclude swipes that work on sphere
            if (!lastRaDec || std::abs(raDec->getRaDegrees() - lastRaDec->getRaDegrees()) < 90.0) {
                m_points.push_back(raDec);
            }
            lastRaDec = raDec;
        }
        else {
            std::cout << "Reading poly unexpected coord count " << coordComp << " intensity " << m_intensity << std::endl;
        }
    }
}