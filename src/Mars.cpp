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


#include "Mars.hpp"

Mars::Mars()
: Planet()
{
}

std::string
Mars::getName()
{
	return "Mars";
}

std::array<double,6>
Mars::getElements()
{
    return std::array<double,6>{ 1.52371034,      0.09339410,      1.84969142,       -4.55343205,    -23.94362959,     49.55953891};
}

std::array<double,6>
Mars::getRates()
{
    return std::array<double,6>{ 0.00001847,      0.00007882,     -0.00813131,    19140.30268499,      0.44441088,     -0.29257343};
}