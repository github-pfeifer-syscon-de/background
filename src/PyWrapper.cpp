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
#include <cstdio>
#include <marshal.h>
#include <psc_format.hpp>

#include "FileLoader.hpp"
#include "PyWrapper.hpp"

PyClass::PyClass(const std::string& obj, const std::string& src)
: m_obj{obj}
, m_src{src}
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
    auto info = m_pyFile->query_info(G_FILE_ATTRIBUTE_TIME_MODIFIED);
    auto fileModified = info->get_modification_date_time();
    if (fileModified.compare(m_pySoureModified) > 0) {
        return true;
    }
    if (m_localPyFile && m_localPyFile->query_exists() && !m_pyFile->equal(m_localPyFile)) {
        return true;    // switch from global to local file
    }
    return false;   // no change detected
}

PyObject*
PyClass::compile(const Glib::RefPtr<Gio::File>& pyFile, const Glib::RefPtr<Gio::File>& pycFile)
{
    // failed to background this
    //    the hints from https://awasu.com/weblog/embedding-python/threads/ seem a bit heavy to implement
    std::vector<char> bytes;
    if (!FileLoader::readFile(pyFile, bytes)) {
        setPyError(psc::fmt::format("Error loading source {}", pyFile->get_path()));
        return nullptr;
    }
    // to use file level includes compile is required see https://stackoverflow.com/questions/3789881/create-and-call-python-function-from-string-via-c-api @fridgerator
    PyObject* pCodeObj = Py_CompileString(bytes.data(), m_src.c_str(), Py_file_input);
    if (pCodeObj == nullptr) {  // if the Python syntax is wrong, for example
        setPyError("Error compiling");
        return nullptr;
    }
    // this seems to be right stage to save .pyc
    //   the command "python -m py_compile info.py" produces the same output (used with version 1)
    auto pcyPath = pycFile->get_path();
    FILE* cfile = std::fopen(pcyPath.c_str(), "wb");
    if (cfile) {
        // Version 0 - Historical format
        // Version 1 - Shares interned strings
        // Version 2 - Uses a binary format for floating point numbers
        // Version 3 - Support for object instancing and recursion
        // Version 4 - Current Version (as we use the files locally)
        PyMarshal_WriteObjectToFile(pCodeObj, cfile, Py_MARSHAL_VERSION);
        fclose(cfile);
    }
    return pCodeObj;
}

void
PyClass::setPyError(const Glib::ustring& location)
{
    m_failed = true;
    m_error = location;
    if (PyErr_Occurred()) {
        // see https://stackoverflow.com/questions/1418015/how-to-get-python-exception-text
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        //pvalue contains error message
        //ptraceback contains stack snapshot and many other information
        //(see python traceback structure)
        //Get error message
        auto pyStr = PyObject_Str(pvalue);
        if (pyStr) {
            auto pyUtf8 = PyUnicode_AsUTF8(pyStr);;
            if (pyUtf8) {
                m_error += "\n";
                m_error += pyUtf8;
                Py_DECREF(pyUtf8);
            }
            Py_DECREF(pyStr);
        }
        PyErr_Print();        // this removes the error!!!
    }
}

void
PyClass::setSourceModified()
{
    auto sourceInfo = m_pyFile->query_info(G_FILE_ATTRIBUTE_TIME_MODIFIED);
    m_pySoureModified = sourceInfo->get_modification_date_time();
}

bool
PyClass::load(const std::shared_ptr<FileLoader>& loader)
{
    m_failed = false;
    PyErr_Clear();
    // first stop check source
    const Glib::RefPtr<Gio::File> localDir = loader->getLocalDir();
    m_localPyFile = localDir->get_child(m_src);
    auto sourceBasename = m_src.substr(0, m_src.find('.'));
    Glib::RefPtr<Gio::File> sourcePy = m_localPyFile;
    if (!sourcePy->query_exists()) {
        sourcePy = loader->findFile(m_src);
    }
    m_pyFile = sourcePy;
    setSourceModified();
    // second stop check compiled
    const Glib::RefPtr<Gio::File> compiledPyc = localDir->get_child(sourceBasename + ".pyc");
    PyObject* pCodeObj{};
    if (compiledPyc->query_exists()) {
        auto compiledInfo = compiledPyc->query_info(G_FILE_ATTRIBUTE_TIME_MODIFIED);
        auto compiledModified = compiledInfo->get_modification_date_time();
        if (compiledModified.compare(m_pySoureModified ) > 0) { // compiled is newer
            auto pcyPath = compiledPyc->get_path();
            FILE* cfile = std::fopen(pcyPath.c_str(), "rb");
            if (cfile) {
                pCodeObj = PyMarshal_ReadLastObjectFromFile(cfile);
                fclose(cfile);
            }
        }
    }
    if (!pCodeObj) {
        pCodeObj = compile(sourcePy, compiledPyc);
        if (!pCodeObj) {
            return false;
        }
    }
    // Create a module by executing the code:
    //   This is the step where the time goes for the first instance
    m_pModule = PyImport_ExecCodeModule(m_obj.c_str(), pCodeObj);
    if (m_pModule == nullptr) {
        setPyError("Error running");
        return false;
    }
    //auto start = std::chrono::steady_clock::now();
    //auto end = std::chrono::steady_clock::now();
    //auto elapsed_seconds = std::chrono::duration_cast<
    //      std::chrono::duration<double>>(end - start).count();
    //std::cout<< "Exec took " << elapsed.count() << "ms" << std::endl;
    //pCodeObj is the result of the executing code, chuck it away because we've only declared a class
    Py_DECREF(pCodeObj);
    PyObject* pClass = PyObject_GetAttrString(m_pModule, m_obj.c_str());
    if (pClass && PyCallable_Check(pClass)) {
        m_pInstance = PyObject_CallObject(pClass, nullptr);
    }
    else {
        setPyError(psc::fmt::format("No class {} found!", m_obj));
        return false;
    }
    if (pClass) {
        Py_XDECREF(pClass);
    }
    return true;
}

Glib::RefPtr<Gio::File>
PyClass::getPyFile()
{
    return m_pyFile;
}

Glib::RefPtr<Gio::File>
PyClass::getLocalPyFile()
{
    return m_localPyFile;
}

bool
PyClass::hasFailed()
{
    return m_failed;
}

Glib::ustring
PyClass::getError()
{
    return m_error;
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
PyWrapper::load(const std::shared_ptr<FileLoader>& loader, const std::string& obj, const std::string& src)
{
    auto tempClass = std::make_shared<PyClass>(obj, src);
    tempClass->load(loader);
    return tempClass;
}

