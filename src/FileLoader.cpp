/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
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



#include "FileLoader.hpp"

FileLoader::FileLoader(const Glib::ustring& startPath)
: m_startPath{startPath}
{
}

Glib::RefPtr<Gio::File>
FileLoader::findFile(const Glib::ustring& name)
{
    auto execDir = Gio::File::create_for_path(m_startPath);
    auto distDir = execDir->get_parent()->get_parent();
    auto resDir = Glib::canonicalize_filename("res", distDir->get_path());
    std::string uname = name;
    auto fullPath = Glib::canonicalize_filename(uname, resDir);
    auto file = Gio::File::create_for_path(fullPath);
    if (file->query_exists()) {
        return file;
    }
    fullPath = Glib::canonicalize_filename(uname, PACKAGE_DATA_DIR);
    file = Gio::File::create_for_path(fullPath);
    if (file->query_exists()) {
        return file;
    }
    return Glib::RefPtr<Gio::File>();
}

Glib::ustring
FileLoader::find(const Glib::ustring& name)
{
    auto file = findFile(name);
    if (file) {
        return file->get_path();
    }
    return "";
}