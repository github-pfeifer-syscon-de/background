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
PyClass::load(std::FILE* fp, const std::string& basename, PyObject* pGlobal)
{
    //Create a new module object
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

long
PyClass::displayClock(const std::string& method, const Cairo::RefPtr<Cairo::Context>& ctx, double radius)
{
    long ret = -1;
    if (!m_pInstance) {
        std::cout << "PyClass::displayClock no instance" << std::endl;
        return ret;
    }
    cairo_t* c_ctx = ctx->cobj();
    PyObject* context = PycairoContext_FromContext(c_ctx, &PycairoContext_Type, NULL);
    // see https://docs.python.org/3/c-api/arg.html#building-values
    //   N -> background: ../cairo/src/cairo.c:523: cairo_destroy: Assertion `CAIRO_REFERENCE_COUNT_HAS_REFERENCE (&cr->ref_count)' failed.
    PyObject* pValue = PyObject_CallMethod(m_pInstance, method.c_str(), "(Od)", context, radius);
    if (pValue)  {
        ret = PyLong_AsLong(pValue);
        Py_DECREF(pValue);
    }
    if (PyErr_Occurred()) {
        PyErr_Print();
    }
    //Get a pointer to the function
    //PyObject* pFunc = PyObject_GetAttrString(pModule, fun.c_str());

    //Double check we have actually found it and it is callable
    //if (!pFunc || !PyCallable_Check(pFunc)) {
    //    if (PyErr_Occurred()) {
    //        PyErr_Print();
    //    }
    //    fprintf(stderr, "Cannot find function \"%s\"\n", fun.c_str());
    //    return 2;
    //}

    //pValue = PyObject_CallObject(pFunc, pArgs);

    return ret;
}

long
PyClass::displayInfo(const std::string& method
                    , const Cairo::RefPtr<Cairo::Context>& ctx
                    , const Glib::ustring& font
                    , const Glib::ustring& netInfo)
{
    long ret = -1;
    if (!m_pInstance) {
        std::cout << "PyClass::displayInfo no instance" << std::endl;
        return ret;
    }
    cairo_t* c_ctx = ctx->cobj();
    PyObject* context = PycairoContext_FromContext(c_ctx, &PycairoContext_Type, NULL);
    // see https://docs.python.org/3/c-api/arg.html#building-values
    //   N -> background: ../cairo/src/cairo.c:523: cairo_destroy: Assertion `CAIRO_REFERENCE_COUNT_HAS_REFERENCE (&cr->ref_count)' failed.
    PyObject* pValue = PyObject_CallMethod(m_pInstance, method.c_str(), "(Oss)", context, font.c_str(), netInfo.c_str());
    if (pValue)  {
        ret = PyLong_AsLong(pValue);
        Py_DECREF(pValue);
    }
    if (PyErr_Occurred()) {
        PyErr_Print();
    }
    return ret;
}

long
PyClass::displayCal(const std::string& method
                    , const Cairo::RefPtr<Cairo::Context>& ctx
                    , const Glib::ustring& font)
{
    long ret = -1;
    if (!m_pInstance) {
        std::cout << "PyClass::displayCal no instance" << std::endl;
        return ret;
    }
    cairo_t* c_ctx = ctx->cobj();
    PyObject* context = PycairoContext_FromContext(c_ctx, &PycairoContext_Type, NULL);
    PyObject* pValue = PyObject_CallMethod(m_pInstance, method.c_str(), "(Os)", context, font.c_str());
    if (pValue)  {
        ret = PyLong_AsLong(pValue);
        Py_DECREF(pValue);
    }
    if (PyErr_Occurred()) {
        PyErr_Print();
    }
    return ret;
}

PyObject*
PyClass::build(const Cairo::RefPtr<Cairo::Context>& ctx, double radius)
{
    PyObject*  pArgs = PyTuple_New(2);
    cairo_t* c_ctx = ctx->cobj();
    PyObject* context = PycairoContext_FromContext(c_ctx, &PycairoContext_Type, NULL);
    // see https://docs.python.org/3/c-api/arg.html#building-values
    PyObject* pValue = Py_BuildValue("O", context);   // not sure this is the best method but it works ;)
    PyTuple_SetItem(pArgs, 0, pValue);
    pValue = PyLong_FromDouble(radius);
    PyTuple_SetItem(pArgs, 1, pValue);
    return pArgs;
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
    bool ret = tempClass->load(fp, file->get_basename(), m_pGlobal);
    fclose(fp);
    if (ret) {
        pyClass = tempClass;
    }
    return pyClass;
}

