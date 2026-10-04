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
#include <psc_i18n.hpp>
#include <KeyfileTableManager.hpp>

#include <Flight.hpp>

class StarWin;
class Flights;
namespace psc::ui {
    class KeyfileTableManager;
}


class IconConverter
: public psc::ui::CustomConverter<Glib::RefPtr<Gdk::Pixbuf>>
{
public:
    IconConverter(Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>>& col)
    : CustomConverter(col)
    {
    }
    virtual ~IconConverter() = default;

    void convert(Gtk::CellRenderer* rend, const Gtk::TreeModel::iterator& iter) override  {
         Glib::RefPtr<Gdk::Pixbuf> pixbuf;
         iter->get_value(m_col.index(), pixbuf);
         auto pixRend = static_cast<Gtk::CellRendererPixbuf*>(rend);
         pixRend->property_pixbuf() = pixbuf;
    }

    Gtk::CellRenderer* createCellRenderer() override {
        return Gtk::manage<Gtk::CellRendererPixbuf>(new Gtk::CellRendererPixbuf());
    }
};

class FlightColumns
: public psc::ui::ColumnRecord
{
public:
    Gtk::TreeModelColumn<Glib::ustring> icao24;
    Gtk::TreeModelColumn<Glib::ustring> callsign;
    Gtk::TreeModelColumn<Glib::ustring> originCountry;
    Gtk::TreeModelColumn<Glib::ustring> timePosition;
    Gtk::TreeModelColumn<Glib::ustring> lastContact;
    Gtk::TreeModelColumn<Glib::ustring> onGround;
    Gtk::TreeModelColumn<double> positionLon;
    Gtk::TreeModelColumn<double> positionLat;
    Gtk::TreeModelColumn<double> distance;
    Gtk::TreeModelColumn<double> baroAltitude;
    Gtk::TreeModelColumn<double> velocity;
    Gtk::TreeModelColumn<double> track;
    Gtk::TreeModelColumn<double> verticalRate;
    Gtk::TreeModelColumn<double> geoAltitude;
    Gtk::TreeModelColumn<Glib::ustring> squake;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> iconAltitude;
    Gtk::TreeModelColumn<PtrFlight> flight;
    FlightColumns()
    {
        add<Glib::ustring>(_("Icao"), icao24);
        add<Glib::ustring>(_("Callsign"), callsign);
        add<Glib::ustring>(_("Origin country"), originCountry);
        add<Glib::ustring>(_("Time position"), timePosition);
        add<Glib::ustring>(_("Last contact"), lastContact);
        add<Glib::ustring>(_("On ground"), onGround);
        add<double>(_("Pos. (lon°)"), positionLon);
        add<double>(_("Pos. (lat°)"), positionLat);
        add<double>(_("Distance (grd.)"), distance);
        add<double>(_("Velocity (m/s)"), velocity);
        add<double>(_("Track (°)"), track);
        add<double>(_("Vertical rate (m/s)"), verticalRate);
        add<double>(_("Barom. altitude (m)"), baroAltitude);
        add<double>(_("Geom. altitude (m)"), geoAltitude);
        add<Glib::ustring>(_("Squake"), squake);
        auto iconConverter = std::make_shared<IconConverter>(iconAltitude);
        add<Glib::RefPtr<Gdk::Pixbuf>>(_("Altitude (icon)"), iconConverter);
        Gtk::TreeModel::ColumnRecord::add(flight);
    }
};

class FlightsDlg
: public Gtk::Dialog
, public FlightsConsumer {
public:
    FlightsDlg(BaseObjectType* cobject
        , const Glib::RefPtr<Gtk::Builder>& builder
        , StarWin* starWin);
    explicit FlightsDlg(const FlightsDlg& other) = delete;
    virtual ~FlightsDlg() = default;

    void on_response(int response_id) override;
    void update(const std::vector<PtrFlight>& flights) override;
    void notifyError(const Glib::ustring& error, int status) override;

    static void show(StarWin* starWin);
    static constexpr auto UPDATE_RATE{std::chrono::seconds(60)};
    static constexpr auto ICON_SIZE{20};
protected:
    void showDetail(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
    void showLink(Glib::ustring& uri);
    bool refresh();
private:
    StarWin* m_starWin;
    std::shared_ptr<FlightColumns> flightColumns;
    Glib::RefPtr<Gtk::ListStore> m_store;
    Glib::RefPtr<Gtk::TreeView> m_list;
    std::shared_ptr<psc::ui::KeyfileTableManager> m_kfTableManager;
    sigc::connection m_timer;
    std::shared_ptr<Flights> m_flightsService;
    std::chrono::seconds m_savedUpdate;
};
