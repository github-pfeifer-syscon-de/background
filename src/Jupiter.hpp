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

class Jupiter
: public Planet
{
public:
    Jupiter();
    explicit Jupiter(const Jupiter& orig) = delete;
    virtual ~Jupiter() = default;

    std::string getName() override;
protected:
    static constexpr Elements jupiterElements = {
        .a{5.20288700_AU},
        .e{0.04838624},
        .I{1.30439695},
        .L{34.39644051},
        .w{14.72847983},
        .O{100.47390909}
    };
    static constexpr Elements jupiterRates = {
        .a{-0.00011607},
        .e{-0.00013253},
        .I{-0.00183714},
        .L{3034.74612775},
        .w{0.21252668},
        .O{0.20469106}
    };

    const Elements& getElements() override;
    const Elements& getRates() override;

};

