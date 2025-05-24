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

#include <StringUtils.hpp>

#include "StarDraw.hpp"
#include "StarWin.hpp"
#include "SysInfo.hpp"
#include "Math.hpp"
#include "FileLoader.hpp"
#include "config.h"

std::string
Module::getName()
{
    return m_name;
}

Gdk::RGBA
Module::getPrimaryColor()
{
    Gdk::RGBA dfltColor{"rgb(50%,50%,50%)"};
    return m_config->getColor(getName().c_str(), COLOR_KEY, dfltColor);
}

void
Module::setPrimaryColor(const Gdk::RGBA& primColor)
{
    m_config->setColor(getName().c_str(), COLOR_KEY, primColor);
}

void
Module::getPrimaryColor(const Cairo::RefPtr<Cairo::Context>& ctx)
{
    auto primColor = getPrimaryColor();
    ctx->set_source_rgb(primColor.get_red(), primColor.get_green(), primColor.get_blue());
}


Glib::ustring
Module::getPosition()
{
    return m_config->getString(getName().c_str(), POS_KEY);
}

void
Module::setPosition(const Glib::ustring& position)
{
    m_config->setString(getName().c_str(), POS_KEY, position);
}

Pango::FontDescription
Module::getFont()
{
    return m_config->getFont(getName().c_str(), FONT_KEY, DEFAULT_FONT);
}

void
Module::setFont(Pango::FontDescription& descr)
{
    m_config->setFont(getName().c_str(), FONT_KEY, descr);
}


void
Module::fillPos(Gtk::ComboBoxText* pos)
{
    pos->append("", "None");
    pos->append(POS_TOP, "Top");
    pos->append(POS_MIDDLE, "Middle");
    pos->append(POS_BOTTOM, "Bottom");
}

std::shared_ptr<PyClass>
Module::checkPyClass(StarDraw* starDraw, const char* className)
{
#   ifdef USE_PYTHON
    if (!m_pyClass || m_pyClass->isUpdated()) {
        m_fileLoader = starDraw->getFileLoader();
        auto infoScript = m_pyWrapper->load(m_fileLoader, className, getPyScriptName());
        if (infoScript) {
            if (infoScript->hasFailed()) {
                std::cout << "Module::checkPyClass has failed err " <<infoScript->getError() << std::endl;
                if (m_pyClass) {
                    m_pyClass->setSourceModified();    // keep old version, but adjust modified so we don't popup again
                }
                starDraw->getWindow()->showMessage(infoScript->getError());
            }
            else {
                m_pyClass = infoScript;
            }
        }
        else {
            std::cout << "Module::checkPyClass noscript "  << std::endl;
        }
    }
#   endif
    return m_pyClass;
}

void
Module::fileChanged(const Glib::RefPtr<Gio::File>& file, const Glib::RefPtr<Gio::File>& changed, Gio::FileMonitorEvent event, StarDraw* starDraw)
{
    //Glib::ustring info;
    //if (event == Gio::FileMonitorEvent::FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED) {
    //    info += " attr";
    //}
    //if (event == Gio::FileMonitorEvent::FILE_MONITOR_EVENT_CHANGES_DONE_HINT) {
    //    info += " done";
    //}
    //if (event == Gio::FileMonitorEvent::FILE_MONITOR_EVENT_CHANGED) {
    //    info += " changed";
    //}
    //if (event == Gio::FileMonitorEvent::FILE_MONITOR_EVENT_DELETED) {
    //    info += " deleted";
    //}
    //if (event == Gio::FileMonitorEvent::FILE_MONITOR_EVENT_MOVED) {
    //    info += " moved";
    //}
    //if (event == Gio::FileMonitorEvent::FILE_MONITOR_EVENT_MOVED_IN) {
    //    info += " in";
    //}
    //if (event == Gio::FileMonitorEvent::FILE_MONITOR_EVENT_MOVED_OUT) {
    //    info += " out";
    //}
    //if (event == Gio::FileMonitorEvent::FILE_MONITOR_EVENT_RENAMED) {
    //    info += " renamed";
    //}
    // events emacs no backup
    //file path /home/rpf/.local/share/background/info.py changed  event  deleted
    //file path /home/rpf/.local/share/background/info.py~ changed  event
    //file path /home/rpf/.local/share/background/info.py~ changed  event  done
    //file path /home/rpf/.local/share/background/info.py changed  event
    //file path /home/rpf/.local/share/background/info.py changed  event  changed
    //file path /home/rpf/.local/share/background/info.py changed  event  done
    //file path /home/rpf/.local/share/background/info.py changed  event  attr
    //file path /home/rpf/.local/share/background/info.py changed  event  changed
    // netbeans
    // file path /home/rpf/.local/share/background/info.py changed  event  changed
    // file path /home/rpf/.local/share/background/info.py changed  event  done
    //std::cout << "file path " << file->get_path()
    //          << " changed " << (changed ? changed->get_path() : "")
    //          << " event " << info
    //          << std::endl;
    auto basename = file->get_basename();
    if (basename == getPyScriptName()
     && event == Gio::FileMonitorEvent::FILE_MONITOR_EVENT_CHANGES_DONE_HINT) {
        starDraw->compute();
    }
}

void
Module::edit(StarDraw* starDraw)
{
#   ifdef USE_PYTHON
    auto localScriptFile = m_pyClass->getLocalPyFile();
    if (!localScriptFile->query_exists()) {
        auto scriptDir = localScriptFile->get_parent();
        if (!scriptDir->query_exists()) {
            scriptDir->make_directory_with_parents();
        }
        auto globalScriptFile = m_pyClass->getPyFile();
        globalScriptFile->copy(localScriptFile);    // for editing create a local copy
    }
    if (!m_fileMonitor) {
        m_fileMonitor = localScriptFile->monitor_file(Gio::FileMonitorFlags::FILE_MONITOR_NONE);
        m_fileMonitor->signal_changed().connect(
            sigc::bind(
                sigc::mem_fun(*this, &Module::fileChanged), starDraw));
    }
    std::vector<std::string> args;
    args.push_back("/usr/bin/xdg-open");
    args.push_back(localScriptFile->get_path());
    GPid pid;
    auto msg = m_fileLoader->run(args, &pid);
    if (!msg.empty()) {
        std::cout << "Error running xdg-open" << std::endl;
    }
#   endif
}

void
Module::stopMonitor()
{
    if (m_fileMonitor) {
        m_fileMonitor->cancel();
        m_fileMonitor.reset();
    }
}

Glib::ustring
Module::getEditInfo()
{
#   ifdef USE_PYTHON
    auto localScriptFile = m_pyClass->getLocalPyFile();
    return Glib::ustring::sprintf("Script (%s)", localScriptFile->query_exists() ? "local" : "global");
#   endif
    return "";
}

