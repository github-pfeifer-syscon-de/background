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


#include "Constellation.hpp"


Constellation::Constellation(const std::string& name)
: m_name{name}
{
}

std::string
Constellation::getName()
{
    return m_name;
}

void
Constellation::addPosition(const std::shared_ptr<RaDec>& raDec, int width)
{
	std::shared_ptr<psc::geom::Polyline> polyline;
	if (m_lines.empty()
	 || m_lines.back()->getWidth() != width) {
	    polyline = std::make_shared<psc::geom::Polyline>(width);
	    m_lines.push_back(polyline);
	}
	else {
	    polyline = m_lines.back();
	}
	polyline->add(raDec);
}

const std::list<std::shared_ptr<psc::geom::Polyline>>
Constellation::getPolylines()
{
	return m_lines;     // c++ equivalent to Collections.unmodifiable ?
}
