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
#include <psc_format.hpp>
#include <StringUtils.hpp>
#include <exception>

#include "FileLoader.hpp"

FileLoader::FileLoader(const Glib::ustring& startPath)
: m_startPath{startPath}
{
}

Glib::RefPtr<Gio::File>
FileLoader::findFile(const Glib::ustring& name)
{
    auto execDir = Gio::File::create_for_path(m_startPath);
    //std::cout << "FileLoader::findFile exc " << execDir->get_path() << std::endl;
    auto distDir = execDir->get_parent()->get_parent();
#   ifdef __WIN32__
    distDir = distDir->get_parent();    // windows places the file in .libs
#   endif
    auto resDir = Glib::canonicalize_filename("res", distDir->get_path());
    std::string uname = name;
    auto fullPath = Glib::canonicalize_filename(uname, resDir);
    auto file = Gio::File::create_for_path(fullPath);
    //std::cout << "FileLoader::findFile check local " << fullPath << " exist " << std::boolalpha << file->query_exists() << std::endl;
    if (file->query_exists()) {
        return file;
    }
    fullPath = Glib::canonicalize_filename(uname, PACKAGE_DATA_DIR);
    file = Gio::File::create_for_path(fullPath);
    //std::cout << "FileLoader::findFile check global " << fullPath << " exist " << std::boolalpha << file->query_exists() << std::endl;
    if (file->query_exists()) {
        return file;
    }
    return Glib::RefPtr<Gio::File>();
}


Glib::RefPtr<Gio::File>
FileLoader::getLocalDir()
{
    std::string uname = "background/";
    auto fullPath = Glib::canonicalize_filename(uname, Glib::get_user_data_dir());
    auto userConfig = Gio::File::create_for_path(fullPath);
    if (!userConfig->query_exists()) {
        userConfig->make_directory_with_parents();
    }
    //std::cout << "FileLoader::getLocalDir  " << userConfig->get_path() << std::endl;
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
FileLoader::readLines(const Glib::RefPtr<Gio::File>& file, size_t expected, const Glib::ustring& encoding, bool useException)
{
    std::vector<Glib::ustring> ret;
    ret.reserve(expected);
    Glib::RefPtr<Gio::FileInputStream> fileStrm;
    try {
        fileStrm = file->read();
        Glib::RefPtr<Gio::InputStream> convStrm;
        if (encoding != "UTF-8") {
            //auto conv = Gio::CharsetConverter::create("UTF-8", encoding); // this is broken
            g_autoptr(GError) err{};
            GCharsetConverter* cc = g_charset_converter_new("UTF-8", encoding.c_str(), &err);
            if (err) {
                std::cout << "Error new converter " << err->message << std::endl;
                if (useException) {
                    throw Glib::Error(err);
                }
                return ret;
            }
            auto conv = Glib::wrap(cc);
            convStrm = Gio::ConverterInputStream::create(fileStrm, conv);
        }
        else {
            convStrm = fileStrm;
        }
        auto dataStrm = Gio::DataInputStream::create(convStrm);
        while (true) {
            std::string line;
            dataStrm->read_line(line);
            ret.push_back(line);
            if (dataStrm->get_available() <= 0u) { // this works after reading, but fails if used in the head of loop!
                break;
            }
        }
        try {
            dataStrm->close();
        }
        catch (const Glib::Error& err) {        // Since the second close seems more important
        }
        fileStrm->close();
    }
    catch (const Glib::Error& err) {
        auto msg = psc::fmt::format("FileLoader::readLines error {}", err.what());
        std::cout << msg << std::endl;
        if (fileStrm) {
            fileStrm->close();
        }
        if (useException) {
            throw;
        }
    }
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
    //std::cout << "FileLoader::readFile  " << file->get_path() << " bytes " << bytes.size() << std::endl;
    return true;
}

static void
child_watch_cb( GPid     pid
              , gint     status
              , gpointer user_data)
{
    //g_message("Child %" G_PID_FORMAT, pid);
    g_autoptr(GError) error = nullptr;
    if (!g_spawn_check_wait_status(status, &error)) {
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
    //   this is also supported Glib::spawn_async meanwhile... (but might not for all distros)
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

LineReaderEnc::LineReaderEnc(const Glib::RefPtr<Gio::File>& file, const Glib::ustring& encoding, bool useException )
: m_useException{useException}
{
    g_autoptr(GError) err{};
    auto filePath = file->get_path();
    m_channel = g_io_channel_new_file(filePath.c_str(), "r", &err);
    if (err) {
        m_next = false;
        std::cout << "LineReaderEnc::LineReaderEnc error " << err->message << " open" << std::endl;
        if (useException) {
            throw Glib::Error(err);
        }
    }
    else {
        if (encoding != "UTF-8") {  // as utf-8 is the default at least with linx
            if (g_io_channel_set_encoding(m_channel, encoding.c_str(), &err) != G_IO_STATUS_NORMAL) {
                m_next = false;
                std::cout << "LineReaderEnc::LineReaderEnc error " << err->message << " encoding " << encoding << std::endl;
                if (useException) {
                    throw Glib::Error(err);
                }
            }
        }
    }
}

LineReaderEnc::~LineReaderEnc()
{
    if (m_channel) {
        g_autoptr(GError) err{};
        g_io_channel_shutdown(m_channel, false, &err);
        g_io_channel_unref(m_channel);
        m_channel = nullptr;
        if (err) {
            std::cout << "LineReaderEnc::~LineReaderEnc error " << err->message << std::endl;
            // throwing excep in destructor is a bad idea
        }
    }
}


bool
LineReaderEnc::next(Glib::ustring& str)
{
    if (m_next) {
        again:
        char* cstr{};
        size_t len;
        g_autoptr(GError) err{};
        GIOStatus status = g_io_channel_read_line(m_channel, &cstr, &len, nullptr, &err);
        switch (status) {
            case G_IO_STATUS_EOF:
                m_next = false;
                str = "";       // in case someone uses it reset
                break;
            case G_IO_STATUS_NORMAL:
                {
                    str = cstr;
                    StringUtils::rtrim(str);    // make compatible to Gio::DataInputStream remove line-endings (also blanks)
                    g_free(cstr);
                }
                break;
            case G_IO_STATUS_AGAIN:
                goto again;
            case G_IO_STATUS_ERROR:
            default:
                m_next = false;
                str = "";       // in case someone uses it reset
                if (err) {
                    std::cout << "LineReaderEnc::readLines next " << err->message << " reading" << std::endl;
                    if (m_useException) {
                        throw Glib::Error(err);
                    }
                }
                break;
        }
    }
    return m_next;
}