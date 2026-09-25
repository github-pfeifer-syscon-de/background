/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2026 RPf
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
#include <memory>

class GeoPaint;

class ConfigGridFlights
: public Gtk::Grid
{
public:
    ConfigGridFlights(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, const std::shared_ptr<GeoPaint>& geoPaint);
    virtual ~ConfigGridFlights() = default;

    void save();
private:
    std::shared_ptr<GeoPaint> m_geoPaint;
    Gtk::ComboBoxText* m_comboService;
    Gtk::SpinButton* m_flightRefresh;
    Gtk::SpinButton* m_flightLon;
    Gtk::SpinButton* m_flightLat;
    Gtk::SpinButton* m_flightBounds;
};
