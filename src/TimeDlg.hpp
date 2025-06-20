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

class StarDraw;

class TimeDlg
: public Gtk::Dialog
{
public:
    TimeDlg(BaseObjectType* cobject
            , const Glib::RefPtr<Gtk::Builder>& builder
            , StarWin* starWin);
    virtual ~TimeDlg() = default;

    static void show(StarWin* starWin);
    GeoPosition getGeoPosition() const;

protected:
    void on_time_changed();
    void on_response(int response_id) override;

private:
    Gtk::SpinButton* m_latitude;
    Gtk::SpinButton* m_longitude;
    Gtk::Calendar* m_calendar;
    Gtk::SpinButton* m_spinH;
    Gtk::SpinButton* m_spinM;
    StarWin* m_starWin;
};

