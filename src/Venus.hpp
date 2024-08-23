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

class Venus
: public Planet
{
public:
    Venus();
    explicit Venus(const Venus& orig) = delete;
    virtual ~Venus() = default;

    std::string getName() override;
protected:
    static constexpr Elements venusElements = {
        .a{0.72333566_AU},
        .e{0.00677672},
        .I{3.39467605},
        .L{181.97909950},
        .w{131.60246718},
        .O{76.67984255}
    };
    static constexpr Elements venusRates = {
        .a{0.00000390},
        .e{-0.00004107},
        .I{-0.00078890},
        .L{58517.81538729},
        .w{0.00268329},
        .O{-0.27769418}
    };

    const Elements& getElements() override;
    const Elements& getRates() override;
};

