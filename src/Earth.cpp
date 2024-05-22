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
#include "Earth.hpp"

Earth::Earth()
: Planet()
{
}


std::string
Earth::getName() {
	return "Earth";
}

std::array<double,6>
Earth::getElements()
{
    return std::array<double,6>{ 1.00000261_AU,      0.01671123,     -0.00001531,      100.46457166,    102.93768193,      0.0};
}

std::array<double,6>
Earth::getRates()
{
    return std::array<double,6>{ 0.00000562,     -0.00004392,     -0.01294668,    35999.37244981,      0.32327364,      0.0};
}
