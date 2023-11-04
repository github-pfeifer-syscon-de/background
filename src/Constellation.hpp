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

#include <string>
#include <list>
#include <memory>

#include "RaDec.hpp"
#include "Polyline.hpp"

class Constellation
{
public:
    Constellation(const std::string& name);
    explicit Constellation(const Constellation& orig) = delete;
    virtual ~Constellation() = default;

    std::string getName();
    void addPosition(const std::shared_ptr<RaDec>& raDec, int width);
    const std::list<std::shared_ptr<Polyline>> getPolylines();

private:
    std::string m_name;
    std::list<std::shared_ptr<Polyline>> m_lines;
};

