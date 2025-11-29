/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * Copyright (C) 2023 RPf 
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

#include <gtkmm.h>
#include <string>
#include <memory>

#include "Constellation.hpp"
#include "RaDec.hpp"

class FileLoader;

class ConstellationFormat
{
public:
    ConstellationFormat(const std::shared_ptr<FileLoader>& fileLoader);
    explicit ConstellationFormat(const ConstellationFormat& orig) = delete;
    virtual ~ConstellationFormat() = default;

    std::list<std::shared_ptr<Constellation>> getConstellations();
private:
    static constexpr auto constlDataFile = "SnT_constellation.txt";
    const std::shared_ptr<FileLoader> m_fileLoader;
    std::list<std::shared_ptr<Constellation>> m_list;
    std::list<std::shared_ptr<Constellation>> readConstellations();
    void parseLine(const std::string& line, std::map<std::string, std::shared_ptr<Constellation>>& constellations);
};

