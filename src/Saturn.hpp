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

class Saturn
: public Planet
{
public:
    Saturn();
    explicit Saturn(const Saturn& orig) = delete;
    virtual ~Saturn() = default;

    std::string getName() override;
protected:
    static constexpr Elements saturnElements = {
        .a{9.53667594_AU},
        .e{0.05386179},
        .I{2.48599187},
        .L{49.95424423},
        .w{92.59887831},
        .O{113.66242448}
    };
    static constexpr Elements saturnRates = {
        .a{-0.00125060},
        .e{-0.00050991},
        .I{0.00193609},
        .L{1222.49362201},
        .w{-0.41897216},
        .O{-0.28867794}
    };
    const Elements& getElements() override;
    const Elements& getRates() override;

};

