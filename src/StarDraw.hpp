/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#pragma once

#include <gtkmm.h>

#include "Layout.hpp"
#include "GeoPosition.hpp"
#include "JulianDate.hpp"
#include "StarPaint.hpp"

class StarWin;
class StarMountOp;

class StarDraw
: public Gtk::DrawingArea
{
public:
    StarDraw(BaseObjectType* cobject
            , const Glib::RefPtr<Gtk::Builder>& builder
            , StarWin* starWin);
    explicit StarDraw(const StarDraw& orig) = delete;
    virtual ~StarDraw() = default;

    void update();
    void update(Glib::DateTime dateTime, GeoPosition& pos);
    void showError(const Glib::ustring& msg, Gtk::MessageType msgType = Gtk::MessageType::MESSAGE_INFO);

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    StarWin* getWindow();

    bool on_button_press_event(GdkEventButton* event) override;
    Gtk::Menu *build_popup();

private:
    Cairo::RefPtr<Cairo::ImageSurface> m_image;
    Glib::DateTime m_displayTimeUtc;
    StarWin* m_starWin;
    PtrStarPaint m_starPaint;
};
