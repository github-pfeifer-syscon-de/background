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
#include <StringUtils.hpp>
#include <py3cairo.h>
#include <cstdio>
#include <marshal.h>

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
    auto info = m_pyFile->query_info(G_FILE_ATTRIBUTE_TIME_MODIFIED);
    auto fileModified = info-> get_modification_date_time();
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
    //    the hints from https://awasu.com/weblog/embedding-python/threads/ didn't help...
    std::vector<char> bytes;
    if (!FileLoader::readFile(pyFile, bytes)) {
        std::cout << "PyClass::load error loading " << pyFile->get_path() << std::endl;
        return nullptr;
    }
    // to use file level includes compile is required see https://stackoverflow.com/questions/3789881/create-and-call-python-function-from-string-via-c-api @fridgerator
    PyObject* pCodeObj = Py_CompileString(bytes.data(), "", Py_file_input);
    //pCodeObj would be null if the Python syntax is wrong, for example
    if (pCodeObj == nullptr) {
        std::cout << "No code obj! (error compiling)" << std::endl;
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        return nullptr;
    }
    // this seems to be right stage to save .pyc
    //   the command "python -m py_compile info.py" produces the same output
    auto pcyPath = pycFile->get_path();
    FILE* cfile = std::fopen(pcyPath.c_str(), "wb");
    if (cfile) {
        // Version 0 - Historical format
        // Version 1 - Shares interned strings
        // Version 2 - Uses a binary format for floating point numbers
        // Version 3 - Support for object instancing and recursion
        // Version 4 - Current Version (as we use the files locally)
        PyMarshal_WriteObjectToFile(pCodeObj, cfile, Py_MARSHAL_VERSION);      // unsure use 2 ?
        fclose(cfile);
    }
    return pCodeObj;
}

bool
PyClass::load(const std::shared_ptr<FileLoader>& loader)
{
    // first stop check source
    const auto sourceBasename = StringUtils::lower(m_obj, 0ul);
    const Glib::RefPtr<Gio::File> localDir = loader->getLocalDir();
    const auto sourceName = sourceBasename + ".py";
    m_localPyFile = localDir->get_child(sourceName);
    Glib::RefPtr<Gio::File> sourcePy = m_localPyFile;
    if (!sourcePy->query_exists()) {
        sourcePy = loader->findFile(sourceName);
    }
    m_pyFile = sourcePy;
    auto sourceInfo = m_pyFile->query_info(G_FILE_ATTRIBUTE_TIME_MODIFIED);
    m_pySoureModified = sourceInfo->get_modification_date_time();
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
            std::cout << "Failed to compile" << std::endl;
            return false;
        }
    }
    // Create a module by executing the code:
    //   This is the step where the time goes for the first instance
    m_pModule = PyImport_ExecCodeModule(m_obj.c_str(), pCodeObj);
    if (m_pModule == nullptr) {
        std::cout << "No module! (error running)" << std::endl;
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
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
        std::cout << "No class " << m_obj << " found!" << std::endl;
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
PyWrapper::load(const std::shared_ptr<FileLoader>& loader, const std::string& obj)
{
    std::shared_ptr<PyClass> pyClass;
    auto tempClass = std::make_shared<PyClass>(obj);
    bool ret = tempClass->load(loader);
    if (ret) {
        pyClass = tempClass;
    }
    return pyClass;
}

