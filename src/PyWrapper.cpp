/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
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

#include <iostream>
#include <string>
#include <py3cairo.h>

#include "FileLoader.hpp"
#include "PyWrapper.hpp"

PyClass::PyClass(const std::string& obj)
: m_obj{obj}
{
}

PyClass::~PyClass()
{
    if (m_pInstance) {
        Py_XDECREF(m_pInstance);  // cleanup instance
    }
    if (m_pModule) {
        Py_DECREF(m_pModule);
    }
}

bool
PyClass::isUpdated()
{
    auto info = m_file->query_info("*");
    auto fileModified = info-> get_modification_date_time();
    if (fileModified.compare(m_fileModified) > 0) {
        return true;
    }
    if (m_localFile && m_localFile->query_exists() && !m_file->equal(m_localFile)) {
        return true;    // switch from global to local file
    }
    return false;   // no change detected
}

bool
PyClass::load(const Glib::RefPtr<Gio::File>& file)
{
    std::vector<char> bytes;
    if (!FileLoader::readFile(file, bytes)) {
        std::cout << "PyClass::load error loading " << file->get_path() << std::endl;
        return false;
    }
    // to use file level includes compile is required see https://stackoverflow.com/questions/3789881/create-and-call-python-function-from-string-via-c-api @fridgerator
    PyObject *pCodeObj = Py_CompileString(bytes.data(), "", Py_file_input);
    //pCodeObj would be null if the Python syntax is wrong, for example
    if (pCodeObj == nullptr) {
        std::cout << "No code obj! (error compiling)" << std::endl;
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        return false;
    }
    //    Create a module by executing the code:
    m_pModule = PyImport_ExecCodeModule(m_obj.c_str(), pCodeObj);
    if (m_pModule == nullptr) {
        std::cout << "No module! (error running)" << std::endl;
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        return false;
    }
    //pCodeObj is the result of the executing code, chuck it away because we've only declared a class
    Py_DECREF(pCodeObj);
    PyObject* pClass = PyObject_GetAttrString(m_pModule, m_obj.c_str());
    if (pClass && PyCallable_Check(pClass)) {
        m_pInstance = PyObject_CallObject(pClass, nullptr);
    }
    else {
        std::cout << "No class " << m_obj << " found!" << std::endl;
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        return false;
    }
    if (pClass) {
        Py_XDECREF(pClass);
    }
    m_file = file;
    auto info = file->query_info("*");
    m_fileModified = info->get_modification_date_time();
    return true;
}

Glib::RefPtr<Gio::File>
PyClass::getFile()
{
    return m_file;
}

void
PyClass::setLocalFile(const Glib::RefPtr<Gio::File>& localFile)
{
    m_localFile = localFile;
}

PyObject*
PyClass::ctx2py(const Cairo::RefPtr<Cairo::Context>& ctx)
{
    cairo_t* c_ctx = ctx->cobj();
    PyObject* context = PycairoContext_FromContext(c_ctx, &PycairoContext_Type, NULL);
    return context;
}

PyWrapper::PyWrapper()
{
    Py_Initialize(); // Initialize the Python Interpreter
    if (import_cairo() < 0) {
       std::cout << "Pycairo not initalized!" << std::endl;
    }
}

PyWrapper::~PyWrapper()
{
    Py_Finalize(); // Clean up and close the Python Interpreter
}

std::shared_ptr<PyClass>
PyWrapper::load(const Glib::RefPtr<Gio::File>& file, const std::string& obj)
{
    std::shared_ptr<PyClass> pyClass;
    auto tempClass = std::make_shared<PyClass>(obj);
    bool ret = tempClass->load(file);
    if (ret) {
        pyClass = tempClass;
    }
    return pyClass;
}

