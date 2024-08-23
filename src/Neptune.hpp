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

class Neptune
: public Planet
{
public:
    Neptune();
    explicit Neptune(const Neptune& orig) = delete;
    virtual ~Neptune() = default;

    std::string getName() override;
protected:
    static constexpr Elements neptuneElements = {
        .a{30.06992276_AU},
        .e{0.00859048},
        .I{1.77004347},
        .L{-55.12002969},
        .w{44.96476227},
        .O{131.78422574}
    };
    static constexpr Elements neptuneRates = {
        .a{0.00026291},
        .e{0.00005105},
        .I{0.00035372},
        .L{218.45945325},
        .w{-0.32241464},
        .O{-0.00508664}
    };

    const Elements& getElements() override;
    const Elements& getRates() override;
};

