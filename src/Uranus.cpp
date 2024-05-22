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
#include "Uranus.hpp"

Uranus::Uranus()
: Planet()
{
}

std::string
Uranus::getName()
{
	return "Uranus";
}

std::array<double,6>
Uranus::getElements() {
    return std::array<double,6>{19.18916464_AU,      0.04725744,      0.77263783,      313.23810451,    170.95427630,     74.01692503};
}

std::array<double,6>
Uranus::getRates()
{
    return std::array<double,6>{-0.00196176,     -0.00004397,     -0.00242939,      428.48202785,      0.40805281,      0.04240589};
}
