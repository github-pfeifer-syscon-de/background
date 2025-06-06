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


#include "Polyline.hpp"
#include "RaDec.hpp"

namespace psc::geom {

Polyline::Polyline(int width)
: m_width{width}
{
}

void
Polyline::add(const std::shared_ptr<RaDec>& raDec)
{
    m_points.push_back(raDec);
}

const std::list<std::shared_ptr<RaDec>>
Polyline::getPoints()
{
    return m_points;
}

int
Polyline::getWidth()
{
    return m_width;
}

} // namespace psc::geom 