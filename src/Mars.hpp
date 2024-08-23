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
class Mars
: public Planet
{
public:
    Mars();
    explicit Mars(const Mars& orig) = delete;
    virtual ~Mars() = default;

    std::string getName() override;
protected:
    static constexpr Elements marsElements = {
        .a{1.52371034_AU},
        .e{0.09339410},
        .I{1.84969142},
        .L{-4.55343205},
        .w{-23.94362959},
        .O{49.55953891}
    };
    static constexpr Elements marsRates = {
        .a{0.00001847},
        .e{0.00007882},
        .I{-0.00813131},
        .L{19140.30268499},
        .w{0.44441088},
        .O{-0.29257343}
    };

    const Elements& getElements() override;
    const Elements& getRates() override;

};

