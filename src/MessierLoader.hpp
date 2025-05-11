/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf <gpl3@pfeifer-syscon.de>
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
#include <JsonHelper.hpp>

#include "FileLoader.hpp"
#include "Messier.hpp"


class MessierLoader
{
public:
    MessierLoader(const std::shared_ptr<FileLoader>& fileLoader);
    explicit MessierLoader(const MessierLoader& orig) = delete;
    virtual ~MessierLoader() = default;

    std::list<std::shared_ptr<Messier>> getMessiers();
    static double toDecimal(const Glib::ustring& xms);
protected:
    std::list<std::shared_ptr<Messier>> readObjects();
    std::shared_ptr<Messier> readMessier(JsonObject* m, JsonHelper& jsonHelper);

private:
    static constexpr auto messierDataFile = "messier.json";
    std::shared_ptr<FileLoader> m_fileLoader;
    std::list<std::shared_ptr<Messier>> m_messier;

};

