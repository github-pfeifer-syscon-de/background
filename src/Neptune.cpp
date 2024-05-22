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
#include "Neptune.hpp"

Neptune::Neptune()
: Planet()

{
}

std::string
Neptune::getName()
{
	return "Neptune";
}

std::array<double,6>
Neptune::getElements()
{
    return std::array<double,6>{30.06992276_AU,      0.00859048,      1.77004347,      -55.12002969,     44.96476227,    131.78422574};
}

std::array<double,6>
Neptune::getRates()
{
    return std::array<double,6>{ 0.00026291,      0.00005105,      0.00035372,      218.45945325,     -0.32241464,     -0.00508664};
}