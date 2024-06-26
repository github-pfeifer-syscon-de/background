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

#include "Math.hpp"
#include "Venus.hpp"

Venus::Venus()
: Planet()
{
}


std::string
Venus::getName()
{
	return "Venus";
}

std::array<double,6> Venus::getElements()
{
	return std::array<double,6>{ 0.72333566_AU,      0.00677672,      3.39467605,      181.97909950,    131.60246718,     76.67984255};
}

std::array<double,6> Venus::getRates()
{
	return std::array<double,6>{ 0.00000390,     -0.00004107,     -0.00078890,    58517.81538729,      0.00268329,     -0.27769418};
}