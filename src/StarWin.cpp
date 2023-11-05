/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2023 RPf <gpl3@pfeifer-syscon.de>
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

#include <cmath>
#include <iostream>

#include "StarWin.hpp"
#include "StarDraw.hpp"

StarWin::StarWin(BaseObjectType* cobject
        , const Glib::RefPtr<Gtk::Builder>& builder
        , const Gtk::Application& appl)
: Gtk::ApplicationWindow(cobject)
{
    set_title("Star");
    auto pix = Gdk::Pixbuf::create_from_resource(appl.get_resource_base_path() + "/background.png");
    set_icon(pix);

    builder->get_widget_derived("drawingArea", m_drawingArea, appl);
    set_decorated(false);
    maximize();
    Glib::DateTime dateTime = Glib::DateTime::create_now_local();
    m_timer = Glib::signal_timeout().connect_seconds(
            sigc::mem_fun(*this, &StarWin::timeoutHandler), 60 - dateTime.get_second());
    signal_hide().connect([this] {
        if (m_timer) {
            m_timer.disconnect();
        }
    });
}

bool
StarWin::timeoutHandler()
{
    if (m_drawingArea) {
        m_drawingArea->queue_draw();
    }
    Glib::DateTime dateTime = Glib::DateTime::create_now_local();
    m_timer = Glib::signal_timeout().connect_seconds(
            sigc::mem_fun(*this, &StarWin::timeoutHandler), 60 - dateTime.get_second());
    return false;
}