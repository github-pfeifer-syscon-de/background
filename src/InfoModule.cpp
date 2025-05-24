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

#include "StarDraw.hpp"
#include "Math.hpp"
#include "FileLoader.hpp"
#include "config.h"

#include "InfoModule.hpp"


Glib::ustring
InfoModule::getPyScriptName()
{
    return "info.py";
}


int
InfoModule::getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    auto infoFont = getFont();
#   ifdef USE_PYTHON
    auto pyClass = checkPyClass(starDraw, pyClassName);
    if (!pyClass) {
        std::cout << "InfoModule::getHeight no Class!" << std::endl;
        return 0;
    }
    auto fontName = infoFont.to_string();
    return static_cast<int>(pyClass->invokeMethod("getHeight", ctx, fontName));
#   else
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(infoFont);
    pangoLayout->set_text("M");
    int width, height;
    pangoLayout->get_pixel_size(width, height);
    return 6 * height;
#   endif
}

std::string
InfoModule::getText(SysInfo& sysInfo)
{
    std::string text;
    text.reserve(512);
    for (auto info : sysInfo.allInfos()) {
        text += info + "\n";
    }
    return text;
}

void
InfoModule::display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    getPrimaryColor(ctx);
    auto infoFont = getFont();
    SysInfo sysInfo;
    auto netInfo = sysInfo.netInfo();
#   ifdef USE_PYTHON
    auto pyClass = checkPyClass(starDraw, pyClassName);
    if (pyClass) {
        auto font = infoFont.to_string();
        pyClass->invokeMethod("draw", ctx, font, netInfo);
    }
    else {
        std::cout << "InfoModule::display no Class!" << std::endl;
    }
#   else
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(infoFont);
    pangoLayout->set_text(getText(sysInfo));
    ctx->move_to(0.0, 0.0);
    pangoLayout->show_in_cairo_context(ctx);
#   endif
}

void
InfoModule::setupParam(const Glib::RefPtr<Gtk::Builder>& builder, StarDraw* starDraw)
{
    builder->get_widget("infoColor", m_infoColor);
    m_infoColor->set_rgba(getPrimaryColor());

    builder->get_widget("infoFont", m_infoFont);
    m_infoFont->set_font_name(getFont().to_string());

    builder->get_widget("infoPos", m_infoPos);
    fillPos(m_infoPos);
    m_infoPos->set_active_id(getPosition());

    Gtk::Button* editInfo;
    builder->get_widget("editInfo", editInfo);
    Gtk::Label* infoLabel;
    builder->get_widget("infoLabel", infoLabel);
#   ifdef USE_PYTHON
    editInfo->signal_clicked().connect(
        sigc::bind(
            sigc::mem_fun(*this, &InfoModule::edit), starDraw));
    infoLabel->set_text(getEditInfo());
#   else
    editInfo->set_sensitive(false);
    infoLabel->set_sensitive(false);
#   endif
}

void
InfoModule::saveParam(bool save)
{
    if (save) {
        setPrimaryColor(m_infoColor->get_rgba());
        Pango::FontDescription infoFont{m_infoFont->get_font_name()};
        setFont(infoFont);
        setPosition(m_infoPos->get_active_id());
    }
    stopMonitor();
}
