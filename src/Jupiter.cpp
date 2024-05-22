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
#include "Jupiter.hpp"

Jupiter::Jupiter()
{

}

std::string
Jupiter::getName() {
	return "Jupiter";
}

std::array<double,6>
Jupiter::getElements() {
    return std::array<double,6>{ 5.20288700_AU,      0.04838624,      1.30439695,       34.39644051,     14.72847983,    100.47390909};
}

std::array<double,6>
Jupiter::getRates() {
    return std::array<double,6>{-0.00011607,     -0.00013253,     -0.00183714,     3034.74612775,      0.21252668,      0.20469106};
}

