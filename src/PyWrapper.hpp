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

#include <gtkmm.h>
#include <Python.h>
#include <cstdlib>
#include <cstdio>

class PyClass
{
public:
    PyClass(const std::string& obj);
    explicit PyClass(const PyClass& orig) = delete;
    virtual ~PyClass();

    bool isUpdated();
    bool load(std::FILE* fp, const Glib::RefPtr<Gio::File>& file, PyObject* pGlobal);
    long displayClockAnalog(const std::string& method
                    , const Cairo::RefPtr<Cairo::Context>& ctx
                    , double radius);
    long displayClockDigital(const std::string& method
                    , const Cairo::RefPtr<Cairo::Context>& ctx
                    , const std::string& font
                    , const std::string& format
                    , double analogRadius);
    long displayInfo(const std::string& method
                    , const Cairo::RefPtr<Cairo::Context>& ctx
                    , const Glib::ustring& font
                    , const Glib::ustring& netInfo);
    long displayCal(const std::string& method
                    , const Cairo::RefPtr<Cairo::Context>& ctx
                    , const Glib::ustring& font);
    PyObject* build(const Cairo::RefPtr<Cairo::Context>& ctx, double radius);
private:
    std::string m_obj;
    PyObject* m_pInstance{nullptr};
    PyObject* pModule{nullptr};
    PyObject* pLocal{nullptr};
    Glib::RefPtr<Gio::File> m_file;
    Glib::DateTime m_fileModified;
};

// this should be keep as singleton
//   as the init is global
class PyWrapper
{
public:
    PyWrapper();
    explicit PyWrapper(const PyWrapper& orig) = delete;
    virtual ~PyWrapper();

    std::shared_ptr<PyClass> load(const Glib::RefPtr<Gio::File>& file, const std::string& obj);
private:
    PyObject* m_pGlobal;
};

