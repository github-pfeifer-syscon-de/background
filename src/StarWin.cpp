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
#include "BackgroundApp.hpp"

StarWin::StarWin(BaseObjectType* cobject
        , const Glib::RefPtr<Gtk::Builder>& builder
        , BackgroundApp* backAppl)
: Gtk::ApplicationWindow(cobject)
, m_backAppl{backAppl}
{
    set_title("Stars");
    auto pix = Gdk::Pixbuf::create_from_resource(m_backAppl->get_resource_base_path() + "/background.png");
    set_icon(pix);

    builder->get_widget_derived("drawingArea", m_drawingArea, this);
    set_decorated(false);
    maximize();
    updateTimer();
    signal_hide().connect([this] {
        if (m_timer.connected()) {
            m_timer.disconnect();
        }
    });
}

BackgroundApp*
StarWin::getBackgroundAppl()
{
    return m_backAppl;
}

void
StarWin::updateTimer()
{
    Glib::DateTime dateTime = Glib::DateTime::create_now_local();
    // aim for next minute change,
    //  with some extra to prevent a underrun
    //  (overall there seems rather be a chance to overshoot especially with load)
    uint32_t intervalMinutes = 1;
    if (m_drawingArea) {
        intervalMinutes = m_drawingArea->getIntervalMinutes();
    }
    auto delay = (intervalMinutes * 60000u + 50u) - static_cast<unsigned int>(dateTime.get_seconds() * 1000.0);
    m_timer = Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &StarWin::timeoutHandler),
            delay);
}

bool
StarWin::timeoutHandler()
{
    if (m_drawingArea) {
        m_drawingArea->compute();
    }
    updateTimer();
    return false;
}