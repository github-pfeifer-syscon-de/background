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

class Earth
: public Planet
{
public:
    Earth();
    explicit Earth(const Earth& orig) = delete;
    virtual ~Earth() = default;

    std::string getName() override;
protected:
    static constexpr Elements earthElements = {
        .a{1.00000261_AU},
        .e{0.01671123},
        .I{-0.00001531},
        .L{100.46457166},
        .w{102.93768193},
        .O{0.0}
    };
    static constexpr Elements earthRates = {
        .a{0.00000562},
        .e{-0.00004392},
        .I{-0.01294668},
        .L{35999.37244981},
        .w{0.32327364},
        .O{0.0}
    };
    const Elements& getElements() override;
    const Elements& getRates() override;

private:

};

