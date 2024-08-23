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

#include <array>

#include "Math.hpp"
#include "Planet.hpp"

class Mercury
: public Planet
{
public:
    Mercury();
    explicit Mercury(const Mercury& orig) = delete;
    virtual ~Mercury() = default;
    std::string getName() override;
protected:
    static constexpr Elements mercuryElements = {
        .a{0.38709927_AU},
        .e{0.20563593},
        .I{7.00497902},
        .L{252.25032350},
        .w{77.45779628},
        .O{48.33076593}
    };
    static constexpr Elements mercuryRates = {
        .a{0.00000037},
        .e{0.00001906},
        .I{-0.00594749},
        .L{149472.67411175},
        .w{0.16047689},
        .O{-0.12534081}
    };
    const Elements& getElements() override;
    const Elements& getRates() override;

};

