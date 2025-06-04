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

#include <iostream>
#include <glibmm.h>
#include <giomm.h>
#include <string>

class FileLoader
{
public:
    FileLoader(const Glib::ustring& startPath);
    explicit FileLoader(const FileLoader& orig) = delete;
    virtual ~FileLoader() = default;

    Glib::ustring find(const Glib::ustring& name);
    [[deprecated("heavy heap usage, see LineReader,LineReaderEnc")]]
    static std::vector<Glib::ustring> readLines(const Glib::RefPtr<Gio::File>& file, size_t expected = 16, const Glib::ustring& encoding = "UTF-8", bool useException = false);
    static bool readFile(const Glib::RefPtr<Gio::File>& file, std::vector<char>& bytes);
    Glib::RefPtr<Gio::File> findFile(const Glib::ustring& name);
    Glib::RefPtr<Gio::File> getLocalDir();
    Glib::ustring run(const std::vector<std::string>& strArgs, GPid* pid);

private:
    Glib::ustring m_startPath;
};

// best use with Ascii/UTF-8
class LineReader
{
public:
    LineReader(const Glib::RefPtr<Gio::File>& file)
    : m_file{file}
    {
    }
    explicit LineReader(const LineReader& other) = delete;
    virtual ~LineReader()
    {
        if (m_dataStrm) {
            try {   // ensure we are completing this
                m_dataStrm->close();
            }
            catch (const Glib::Error& err) {
                std::cout << "Error " << err.what() << " closing " << m_file->get_path() << std::endl;
            }
        }
        if (m_fileStrm) {
            try {   // don't want to crash the app
                m_fileStrm->close();
            }
            catch (const Glib::Error& err) {
                std::cout << "Error " << err.what() << " closing " << m_file->get_path() << std::endl;
            }
        }
    }
    bool hasNext()
    {
        if (!m_fileStrm) {
            m_fileStrm = m_file->read();
            m_dataStrm = Gio::DataInputStream::create(m_fileStrm);
            return true;
        }
        return (m_dataStrm->get_available() > 0u); // this works after reading, but fails if used in the head of loop!
    }
    void next(std::string& line)
    {
        m_dataStrm->read_line(line);
    }

private:
    Glib::RefPtr<Gio::File> m_file;
    Glib::RefPtr<Gio::FileInputStream> m_fileStrm;
    Glib::RefPtr<Gio::DataInputStream> m_dataStrm;
};

// best use if encoding is known
class LineReaderEnc
{
public:
    LineReaderEnc(const Glib::RefPtr<Gio::File>& file, const Glib::ustring& encoding = "UTF-8", bool useException = false);
    explicit LineReaderEnc(const LineReaderEnc& other) = delete;
    virtual ~LineReaderEnc();

    bool next(Glib::ustring& str);
private:
    GIOChannel* m_channel{};
    bool m_next{true};
    bool m_useException;
};