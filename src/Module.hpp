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
#include <KeyConfig.hpp>

#include "FileLoader.hpp"
#include "config.h"

#ifdef USE_PYTHON
#include "PyWrapper.hpp"
#else
class PyWrapper;
class PyClass;
#endif

class StarDraw;

class Module
{
public:
    Module(std::string name, const std::shared_ptr<KeyConfig>& config, std::shared_ptr<PyWrapper> pyWrapper)
    : m_name{name}
    , m_config{config}
    , m_pyWrapper{pyWrapper}
    {
    }
    explicit Module(const Module& orig) = delete;
    virtual ~Module() = default;

    virtual int getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) = 0;
    virtual void display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) = 0;
    virtual void setupParam(const Glib::RefPtr<Gtk::Builder>& builder, StarDraw* starDraw) = 0;
    std::string getName();  // used as config group name
    Gdk::RGBA getPrimaryColor();
    void setPrimaryColor(const Gdk::RGBA& primColor);
    void getPrimaryColor(const Cairo::RefPtr<Cairo::Context>& ctx);
    Glib::ustring getPosition();
    void setPosition(const Glib::ustring& position);
    Pango::FontDescription getFont();
    void setFont(Pango::FontDescription& descr);
    void edit(StarDraw* starDraw);
    void fileChanged(const Glib::RefPtr<Gio::File>& file, const Glib::RefPtr<Gio::File>& changed, Gio::FileMonitorEvent event, StarDraw* starDraw);
    virtual Glib::ustring getPyScriptName() = 0;
    Glib::ustring getEditInfo();
    void stopMonitor();
    static constexpr auto COLOR_KEY{"Color"};
    static constexpr auto POS_KEY{"Pos"};
    static constexpr auto FONT_KEY{"Font"};
    static constexpr auto DEFAULT_FONT{"Sans 12"};
    static constexpr auto POS_TOP{"top"};
    static constexpr auto POS_MIDDLE{"mid"};
    static constexpr auto POS_BOTTOM{"bot"};
    virtual void saveParam(bool save);

protected:
    void fillPos(Gtk::ComboBoxText* pos);
    std::shared_ptr<PyClass> checkPyClass(StarDraw* starDraw, const char* className);
    void setupParam(const Glib::RefPtr<Gtk::Builder>& builder
            , StarDraw* starDraw
            , const char* colorId
            , const char* fontId
            , const char* posId
            , const char* editId
            , const char* editLabelId);

    const std::string m_name;
    Glib::ustring m_position;
    Gdk::RGBA m_primaryColor;
    std::shared_ptr<KeyConfig> m_config;
    std::shared_ptr<PyWrapper> m_pyWrapper;
    std::shared_ptr<FileLoader> m_fileLoader;
    std::shared_ptr<PyClass> m_pyClass;
    Glib::RefPtr<Gio::FileMonitor> m_fileMonitor;
    Gtk::ColorButton* m_color;
    Gtk::FontButton* m_font;
    Gtk::ComboBoxText* m_pos;
};

using PtrModule = std::shared_ptr<Module>;


