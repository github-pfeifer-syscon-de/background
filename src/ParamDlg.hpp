/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2018 rpf
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

#include "ConfigGeoJsonGrid.hpp"

class StarDraw;
class GeoPosition;
class ConfigWeatherGrid;
class ConfigGridFlights;

class ParamDlg
: public Gtk::Dialog
{
public:
    ParamDlg(BaseObjectType* cobject
            , const Glib::RefPtr<Gtk::Builder>& builder
            , StarWin* starWin);
    virtual ~ParamDlg() = default;


protected:
    void on_response(int response_id) override;

private:
    StarWin* m_starWin;
    Gtk::SpinButton* m_updateInterval;
    Gtk::ColorButton* m_startColor;
    Gtk::ColorButton* m_stopColor;
    Gtk::FontButton* m_starFont;
    Gtk::CheckButton* m_showMilkyway;
    Gtk::Scale* m_messierVMag;
    Gtk::ComboBoxText* m_display;
    ConfigWeatherGrid* m_configWeatherGrid;
    ConfigGeoJsonGrid* m_configGeoJson;
    ConfigGridFlights* m_flightGrid;
};
