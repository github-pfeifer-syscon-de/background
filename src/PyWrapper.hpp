/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf
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

#include <gtkmm.h>
#include <Python.h>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <algorithm>
#include <psc_format.hpp>

#include "FileLoader.hpp"

class PyClass
{
public:
    PyClass(const std::string& obj, const std::string& src);
    explicit PyClass(const PyClass& orig) = delete;
    virtual ~PyClass();

    bool isUpdated();
    bool load(const std::shared_ptr<FileLoader>& loader);
    PyObject* compile(const Glib::RefPtr<Gio::File>& file, const Glib::RefPtr<Gio::File>& pycFile);

    long invokeMethod(const std::string& method, auto&&... ppargs)
    {
        long ret = -1;
        if (!m_pInstance) {
            std::cout << "PyClass::invokeMethod no instance" << std::endl;
            return ret;
        }
        m_failed = false;
        PyErr_Clear();
        static const std::size_t values = sizeof...(ppargs);
        PyObject* pyArgs = PyTuple_New(values);
        buildArgsAsPyTuple(pyArgs, 0, ppargs...);
        PyObject* pValue = PyObject_CallMethod(m_pInstance, method.c_str(), "O", pyArgs);
        if (pValue)  {
            ret = PyLong_AsLong(pValue);
            Py_DECREF(pValue);
        }
        if (PyErr_Occurred()) {
            setPyError(psc::fmt::format("invoke {} on {}", method, m_obj));
        }
        return ret;
    }
    Glib::RefPtr<Gio::File> getLocalPyFile();
    Glib::RefPtr<Gio::File> getPyFile();
    bool hasFailed();
    Glib::ustring getError();
    void setSourceModified();
protected:
    PyObject* ctx2py(const Cairo::RefPtr<Cairo::Context>& ctx);
    void setPyError(const Glib::ustring& location);

    void
    buildArgsAsPyTuple(PyObject* pyArgs, int pos)
    { // finally the end of arguments ...
    }

    void
    buildArgsAsPyTuple(PyObject* pyArgs, int pos, const Cairo::RefPtr<Cairo::Context>& ctx,  auto&&... ppargs)
    {
        PyObject* pValue = Py_BuildValue("O", ctx2py(ctx));
        PyTuple_SetItem(pyArgs, pos, pValue);
        buildArgsAsPyTuple(pyArgs, pos + 1, ppargs...);
    }

    void
    buildArgsAsPyTuple(PyObject* pyArgs, int pos, const std::string& s, auto&&... ppargs)
    {
        auto pValue = PyUnicode_FromString(s.c_str());
        PyTuple_SetItem(pyArgs, pos, pValue);
        buildArgsAsPyTuple(pyArgs, pos + 1, ppargs...);
    }

    void
    buildArgsAsPyTuple(PyObject* pyArgs, int pos, double d, auto&&... ppargs)
    {
        auto pValue = PyFloat_FromDouble(d);
        PyTuple_SetItem(pyArgs, pos, pValue);
        buildArgsAsPyTuple(pyArgs, pos + 1, ppargs...);
    }

    void
    buildArgsAsPyTuple(PyObject* pyArgs, int pos, long l, auto&&... ppargs)
    {
        auto pValue = PyLong_FromLong(l);
        PyTuple_SetItem(pyArgs, pos, pValue);
        buildArgsAsPyTuple(pyArgs, pos + 1, ppargs...);
    }

    void
    buildArgsAsPyTuple(PyObject* pyArgs, int pos, unsigned long ul, auto&&... ppargs)
    {
        auto pValue = PyLong_FromUnsignedLong(ul);
        PyTuple_SetItem(pyArgs, pos, pValue);
        buildArgsAsPyTuple(pyArgs, pos + 1, ppargs...);
    }

    void
    buildArgsAsPyTuple(PyObject* pyArgs, int pos, bool b, auto&&... ppargs)
    {
        auto pValue = b ? Py_True : Py_False;
        PyTuple_SetItem(pyArgs, pos, pValue);
        buildArgsAsPyTuple(pyArgs, pos + 1, ppargs...);
    }

private:
    const std::string m_obj;
    const std::string m_src;
    PyObject* m_pInstance{nullptr};
    PyObject* m_pModule{nullptr};
    Glib::RefPtr<Gio::File> m_pyFile;
    Glib::RefPtr<Gio::File> m_localPyFile;
    Glib::DateTime m_pySoureModified;
    bool m_failed{false};
    Glib::ustring  m_error;
};

// this should be keep as singleton
//   as the required python init is global
class PyWrapper
{
public:
    PyWrapper();
    explicit PyWrapper(const PyWrapper& orig) = delete;
    virtual ~PyWrapper();

    std::shared_ptr<PyClass> load(const std::shared_ptr<FileLoader>& loader, const std::string& obj, const std::string& src);
private:
};

