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

#include <iostream>


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

Glib::RefPtr<Gio::File>
FileLoader::findLocalFile(const Glib::ustring& name)
{
    auto userConfig = findLocalFileOnly(name);
    if (userConfig->query_exists()) {
        return userConfig;
    }
    return findFile(name);  // try remaining locations
}

Glib::RefPtr<Gio::File>
FileLoader::findLocalFileOnly(const Glib::ustring& name)
{
    std::string uname = "background/" + name;
    auto fullPath = Glib::canonicalize_filename(uname, Glib::get_user_data_dir());
    auto userConfig = Gio::File::create_for_path(fullPath);
    return userConfig;
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


std::vector<Glib::ustring>
FileLoader::readLines(const Glib::RefPtr<Gio::File>& file)
{
    std::vector<Glib::ustring> ret;
    ret.reserve(64);
    auto fileStrm = file->read();
    auto dataStrm = Gio::DataInputStream::create(fileStrm);
    while (true) {
        std::string line;
        dataStrm->read_line_utf8(line);
        ret.push_back(line);
        if (dataStrm->get_available() <= 0u) { // this works after reading, but fails if used in the head of loop!
            break;
        }
    }
    dataStrm->close();
    fileStrm->close();
    return ret;
}

// Glib::file_read_content may be an alternative ...
bool
FileLoader::readFile(const Glib::RefPtr<Gio::File>& file, std::vector<char>& bytes)
{
    auto info = file->query_info("*");
    auto size = info->get_size();
    if (size <= 0) {
        std::cout << "FileLoader::readFile file " << file->get_path() << " was not found? (size = 0)" << std::endl;
        return false;
    }
    bytes.resize(size+1);
    gssize read{};
    try {
        auto fis = file->read();
        read = fis->read(const_cast<char*>(bytes.data()), size);
        fis->close();
    }
    catch (const Glib::Error& exc) {
        std::cout << "FileLoader::readFile error " << exc.what() << " reading file " << file->get_path() << "!" << std::endl;
        return false;
    }
    bytes[read] = '\0';
    return true;
}

static void
child_watch_cb( GPid     pid
              , gint     status
              , gpointer user_data)
{
    g_message("Child %" G_PID_FORMAT, pid);
    g_autoptr(GError) error = nullptr;
    if (g_spawn_check_wait_status(status, &error)) {
        // went fine ...
    }
    else {
        //StarWin* starWin = static_cast<StarWin*>(user_data);
        auto msg = Glib::ustring::sprintf("Open failed with %s", error->message);
        //if (starWin) {
        //    starWin->showMessage(msg, Gtk::MessageType::MESSAGE_ERROR);
        //}
        //else {
        std::cout << msg << std::endl;
        //}
    }
    // Free any resources associated with the child here, such as I/O channels
    g_spawn_close_pid(pid);
}

Glib::ustring
FileLoader::run(const std::vector<std::string>& strArgs, GPid* pid)
{
    std::vector<char *> args;
    args.reserve(strArgs.size() + 1);
    for (auto& str : strArgs) {
        args.push_back(const_cast<char*>(str.c_str()));    // the api is definied this way...
    }
    args.push_back(nullptr);
    g_autoptr(GError) error = nullptr;
    // Spawn child process.
    g_spawn_async(nullptr   // working dir
                 , &args[0] // arguments
                 , nullptr  // envptr
                 , G_SPAWN_DO_NOT_REAP_CHILD
                 , nullptr  // childsetup
                 , this     // user data
                 , pid
                 , &error);
    if (error) {
        return error->message;
    }
    else {
        g_child_watch_add(*pid, child_watch_cb, this);
    }
    return "";
}