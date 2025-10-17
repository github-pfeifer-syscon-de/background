/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
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

#include <iostream>

#include "StarWin.hpp"
#include "Math.hpp"
#include "config.h"

#include "InfoModule.hpp"


Glib::ustring
InfoModule::getPyScriptName()
{
    return "info.py";
}


int
InfoModule::getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarWin* starWin)
{
    auto infoFont = getFont();
#   ifdef USE_PYTHON
    auto pyClass = checkPyClass(starWin, pyClassName);
    if (!pyClass) {
        std::cout << "InfoModule::getHeight no Class!" << std::endl;
        return 0;
    }
    auto fontName = infoFont.to_string();
    auto height = static_cast<int>(pyClass->invokeMethod("getHeight", ctx, fontName));
    if (pyClass->hasFailed()) {
        starWin->showMessage(pyClass->getError(), Gtk::MessageType::MESSAGE_ERROR);
        height = 0;
    }
    return height;
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
InfoModule::display(const Cairo::RefPtr<Cairo::Context>& ctx, StarWin* starWin)
{
    getPrimaryColor(ctx);
    auto infoFont = getFont();
    SysInfo sysInfo;
    auto netInfo = sysInfo.netInfo();
#   ifdef USE_PYTHON
    auto pyClass = checkPyClass(starWin, pyClassName);
    if (pyClass) {
        auto font = infoFont.to_string();
        pyClass->invokeMethod("draw", ctx, font, netInfo);
        if (pyClass->hasFailed()) {
            starWin->showMessage(pyClass->getError(), Gtk::MessageType::MESSAGE_ERROR);
        }
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
InfoModule::setupParam(const Glib::RefPtr<Gtk::Builder>& builder, StarWin* starWin)
{
    Module::setupParam(builder, starWin, "infoColor", "infoFont", "infoPos", "editInfo", "infoLabel");
}
