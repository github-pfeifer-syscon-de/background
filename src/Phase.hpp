/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#pragma once

/**
 * see
 * https://openstax.org/books/astronomy-2e/pages/4-5-phases-and-motions-of-the-moon
 **/
class Phase
{
public:
    explicit Phase(const Phase& orig) = default;
    virtual ~Phase() = default;

    Phase(double i);

    // relative values 1 (full)..0 (new)
    double getPhase() const;

    // this is not linear as phase is projected onto a sphere 1 full .. 0 new
    double getIlluminated() const;

    // true waning (growing), false waxing (shrinking)
    bool isWanning() const;

private:
    double i;		// values 0..pi..2pi, full -> new -> full

    double constrain(double d);

};

