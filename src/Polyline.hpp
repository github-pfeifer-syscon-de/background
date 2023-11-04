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

#include <list>
#include <memory>

class RaDec;

class Polyline
{
public:
    Polyline(int width);
    explicit Polyline(const Polyline& orig) = delete;
    virtual ~Polyline() = default;

    void add(const std::shared_ptr<RaDec>& raDec);
    const std::list<std::shared_ptr<RaDec>> getPoints();
    int getWidth();

private:
    std::list<std::shared_ptr<RaDec>> m_points;
    int m_width;
};

