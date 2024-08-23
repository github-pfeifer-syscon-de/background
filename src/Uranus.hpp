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

#include "Planet.hpp"

class Uranus
: public Planet
{
public:
    Uranus();
    explicit Uranus(const Uranus& orig) = delete;
    virtual ~Uranus() = default;

    std::string getName() override;
protected:
    static constexpr Elements uranusElements = {
        .a{19.18916464_AU},
        .e{0.04725744},
        .I{0.77263783},
        .L{313.23810451},
        .w{170.95427630},
        .O{74.01692503}
    };
    static constexpr Elements uranusRates = {
        .a{-0.00196176},
        .e{-0.00004397},
        .I{-0.00242939},
        .L{428.48202785},
        .w{0.40805281},
        .O{0.04240589}
    };
    const Elements& getElements() override;
    const Elements& getRates() override;

};

