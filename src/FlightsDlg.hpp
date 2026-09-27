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

#include <Flight.hpp>

class StarWin;

class FlightColumns
: public Gtk::TreeModel::ColumnRecord
{
public:
    Gtk::TreeModelColumn<Glib::ustring> icao24;
    Gtk::TreeModelColumn<Glib::ustring> callsign;
    Gtk::TreeModelColumn<Glib::ustring> originCountry;
    Gtk::TreeModelColumn<Glib::ustring> timePosition;
    Gtk::TreeModelColumn<Glib::ustring> lastContact;
    Gtk::TreeModelColumn<double> positionLon;
    Gtk::TreeModelColumn<double> positionLat;
    Gtk::TreeModelColumn<double> distance;
    Gtk::TreeModelColumn<double> baroAltitude;
    Gtk::TreeModelColumn<double> velocity;
    Gtk::TreeModelColumn<double> track;
    Gtk::TreeModelColumn<double> verticalRate;
    Gtk::TreeModelColumn<double> geoAltitude;
    Gtk::TreeModelColumn<Glib::ustring> squake;
    Gtk::TreeModelColumn<PtrFlight> flight;
    FlightColumns()
    {
        add(icao24);
        add(callsign);
        add(originCountry);
        add(timePosition);
        add(lastContact);
        add(positionLon);
        add(positionLat);
        add(distance);
        add(baroAltitude);
        add(velocity);
        add(track);
        add(verticalRate);
        add(geoAltitude);
        add(squake);
        add(flight);
    }
};

class FlightsDlg
: public Gtk::Dialog
, public FlightsConsumer
{
public:
    FlightsDlg(BaseObjectType* cobject
        , const Glib::RefPtr<Gtk::Builder>& builder
        , StarWin* starWin);
    explicit FlightsDlg(const FlightsDlg& other) = delete;
    virtual ~FlightsDlg() = default;

    void update(const std::vector<PtrFlight>& flights) override;
    void notifyError(const Glib::ustring& error, int status) override;

    static void show(StarWin* starWin);
protected:
    FlightColumns flightColumns;
    void showDetail(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
    void showLink(Glib::ustring& uri);

private:
    StarWin* m_starWin;
    Glib::RefPtr<Gtk::ListStore> m_store;
    Gtk::TreeView* m_list;
};
