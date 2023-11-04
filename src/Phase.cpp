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

#include <cmath>

#include "Phase.hpp"


Phase::Phase(double _i)
{
    i = _i;
}

double
Phase::getPhase() const
{
    return std::abs(M_PI - i) / M_PI;
}

// this is not linear as phase is projected onto a sphere 1 full .. 0 new
double
Phase::getIlluminated() const
{
    double k = (1.0 + std::cos(i)) / 2.0;
    return k;
}

// true waning (growing), false waxing (shrinking)
bool
Phase::isWanning() const
{
    return i >= M_PI;
}

