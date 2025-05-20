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

#include "PyWrapper.hpp"

PyClass::PyClass(const std::string& obj)
: m_obj{obj}
{
}

PyClass::~PyClass()
{
    Py_XDECREF(m_pInstance);  // cleanup instance
    Py_DECREF(pLocal);
    Py_DECREF(pModule);
}

bool
PyClass::isUpdated()
{
    auto info = m_file->query_info("*");
    auto fileModified = info-> get_modification_date_time();
    return fileModified.compare(m_fileModified) > 1;
}

bool
PyClass::load(std::FILE* fp, const Glib::RefPtr<Gio::File>& file, PyObject* pGlobal)
{
    m_file = file;
    auto info = m_file->query_info("*");
    m_fileModified = info-> get_modification_date_time();
    //Create a new module object
    std::string basename = file->get_basename();
    pModule = PyModule_New(m_obj.c_str());
    PyModule_AddStringConstant(pModule, "__file__", basename.c_str());
    //Get the dictionary object from my module so I can pass this to PyRun_String
    pLocal = PyModule_GetDict(pModule);
    PyObject* pValue = PyRun_File(fp, basename.c_str(), Py_file_input, pGlobal, pLocal);
    //pValue would be null if the Python syntax is wrong, for example
    if (pValue == nullptr) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        return false;
    }
    //pValue is the result of the executing code, chuck it away because we've only declared a class
    Py_DECREF(pValue);
    PyObject* pClass = PyDict_GetItemString(pLocal, m_obj.c_str());
    if (pClass && PyCallable_Check(pClass)) {
        m_pInstance = PyObject_CallObject(pClass, NULL);
    }
    else {
        fprintf(stdout, "The class %s is not callable.\n", m_obj.c_str());
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        return false;
    }
    if (pClass) {
        Py_XDECREF(pClass);
    }

    return true;
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
       fprintf(stderr, "Pycairo not initalized ...\n");
    }
    m_pGlobal = PyDict_New();
}

PyWrapper::~PyWrapper()
{
    Py_DECREF(m_pGlobal);
    Py_Finalize(); // Clean up and close the Python Interpreter
}

std::shared_ptr<PyClass>
PyWrapper::load(const Glib::RefPtr<Gio::File>& file, const std::string& obj)
{
    std::shared_ptr<PyClass> pyClass;
    auto fp = std::fopen(file->get_path().c_str(), "r");
    if (!fp) {
        std::cout << "The  file " << file->get_path() << " was not opened!" << std::endl;
        return pyClass;
    }
    auto tempClass = std::make_shared<PyClass>(obj);
    bool ret = tempClass->load(fp, file, m_pGlobal);
    fclose(fp);
    if (ret) {
        pyClass = tempClass;
    }
    return pyClass;
}

