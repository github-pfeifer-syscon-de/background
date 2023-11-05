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


#include "Mercury.hpp"

Mercury::Mercury()
: Planet()
{
}

std::string Mercury::getName() {
	return "Mercury";
}

std::array<double,6>
Mercury::getElements()
{
    return std::array<double,6> { 0.38709927,      0.20563593,      7.00497902,      252.25032350,     77.45779628,     48.33076593};
}

std::array<double,6>
Mercury::getRates()
{
    return std::array<double,6> { 0.00000037,      0.00001906,     -0.00594749,   149472.67411175,      0.16047689,     -0.12534081};
}
