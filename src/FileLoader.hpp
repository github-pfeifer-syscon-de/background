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

#include <glibmm.h>
#include <giomm.h>

class FileLoader
{
public:
    FileLoader(const Glib::ustring& startPath);
    explicit FileLoader(const FileLoader& orig) = delete;
    virtual ~FileLoader() = default;

    Glib::ustring find(const Glib::ustring& name);
    static std::vector<Glib::ustring> readLines(const Glib::RefPtr<Gio::File>& file);
    static bool readFile(const Glib::RefPtr<Gio::File>& file, std::vector<char>& bytes);
    Glib::RefPtr<Gio::File> findFile(const Glib::ustring& name);
    Glib::RefPtr<Gio::File> getLocalDir();
    Glib::ustring run(const std::vector<std::string>& strArgs, GPid* pid);

private:
    Glib::ustring m_startPath;
};

