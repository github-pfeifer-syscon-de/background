/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2026 RPf
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

// tested for german gn250 data
//   as it seems these do not contain more information than the dbf
//    ..._p has just points
//    ..._b has simple bounding boxes as 20bytes -4 shape type = 2 positions
class ShapeFile {
public:
    ShapeFile(const std::string& dir);
    explicit ShapeFile(const ShapeFile& other) = delete;
    virtual ~ShapeFile() = default;
};
