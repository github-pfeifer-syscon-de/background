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
#include "Saturn.hpp"

Saturn::Saturn()
: Planet()
{
}

std::string
Saturn::getName()
{
	return "Saturn";
}

std::array<double,6>
Saturn::getElements()
{
    return std::array<double,6> { 9.53667594_AU,      0.05386179,      2.48599187,       49.95424423,     92.59887831,    113.66242448};
}

std::array<double,6>
Saturn::getRates()
{
    return std::array<double,6> {-0.00125060,     -0.00050991,      0.00193609,     1222.49362201,     -0.41897216,     -0.28867794};
}
