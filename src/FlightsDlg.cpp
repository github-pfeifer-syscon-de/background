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


#include <Flight.hpp>
#include <psc_i18n.hpp>

#include "GeoPaint.hpp"
#include "BackgroundApp.hpp"
#include "StarWin.hpp"
#include "FlightsDlg.hpp"

#include "BackConfig.hpp"

FlightsDlg::FlightsDlg(
          BaseObjectType* cobject
        , const Glib::RefPtr<Gtk::Builder>& builder
        , StarWin* starWin)
: Gtk::Dialog(cobject)
, m_starWin{starWin}
{
    builder->get_widget("list", m_list);
    m_store = Gtk::ListStore::create(flightColumns);
    m_list->append_column(_("Icao"), flightColumns.icao24);
    m_list->append_column(_("Callsign"), flightColumns.callsign);
    m_list->append_column(_("Origin country"), flightColumns.originCountry);
    m_list->append_column(_("Time position"), flightColumns.timePosition);
    m_list->append_column(_("Last contact"), flightColumns.lastContact);
    m_list->append_column(_("Pos. (lat°)"), flightColumns.positionLat);
    m_list->append_column(_("Pos. (lon°)"), flightColumns.positionLon);
    m_list->append_column(_("Distance"), flightColumns.distance);
    m_list->append_column(_("Velocity (m/s)"), flightColumns.velocity);
    m_list->append_column(_("Track (°)"), flightColumns.track);
    m_list->append_column(_("Vertical rate (m/s)"), flightColumns.verticalRate);
    m_list->append_column(_("Barom. altitude (m)"), flightColumns.baroAltitude);
    m_list->append_column(_("Geom. altitude (m)"), flightColumns.geoAltitude);
    m_list->append_column(_("Squake"), flightColumns.squake);
    m_list->set_model(m_store);
    m_list->signal_row_activated().connect(
        sigc::mem_fun(*this, &FlightsDlg::showDetail));
    auto geoPaint = m_starWin->getGeoPaint();
    auto flightsService = geoPaint->getFlightService();
    if (flightsService) {
        flightsService->addListener(this);
    }
    else {
        m_starWin->showMessage(_("No flight service found, check config."));
    }
}

void
FlightsDlg::update(const std::vector<PtrFlight>& flights)
{
    m_store->clear();   // remove previous entries
    auto conf = m_starWin->getConfig();
    auto flightLon = conf->getFlightLongitude();
    auto flightLat = conf->getFlightLatitude();
    GeoCoordinate centCoord = GeoCoordinate(flightLon, flightLat, CoordRefSystem(CoordRefSystem::Value::CRS_84));
    for (auto& flight : flights) {
        auto ins = m_store->append();
        auto row = *ins;
        row.set_value(flightColumns.icao24, Glib::ustring{flight->getIcao24()});
        row.set_value(flightColumns.callsign, Glib::ustring{flight->getCallsign()});
        row.set_value(flightColumns.originCountry, Glib::ustring{flight->getOriginCountry()});
        row.set_value(flightColumns.timePosition, flight->getTimePosition().format_iso8601());
        row.set_value(flightColumns.lastContact, flight->getLastContact().format_iso8601());
        auto gpos = flight->getPosition();
        row.set_value(flightColumns.positionLon,  gpos.getLongitude());
        row.set_value(flightColumns.positionLat, gpos.getLatitude());
        auto dist = centCoord.distance(gpos);
        row.set_value(flightColumns.distance, dist);
        row.set_value(flightColumns.baroAltitude, flight->getBaroAltitude());
        row.set_value(flightColumns.velocity, flight->getVelocity());
        row.set_value(flightColumns.track, flight->getTrack());
        row.set_value(flightColumns.verticalRate, flight->getVerticalRate());
        row.set_value(flightColumns.geoAltitude, flight->getGeoAltitude());
        row.set_value(flightColumns.squake, Glib::ustring{flight->getSquake()});
        row.set_value(flightColumns.flight, flight);
    }
}

void
FlightsDlg::notifyError(const Glib::ustring& error, int status)
{
    m_starWin->showMessage(Glib::ustring::sprintf(_("Flights error %s status %d"), error, status));
}

void
FlightsDlg::showDetail(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column)
{
    auto iter = m_store->get_iter(path);
    auto row = *iter;
    auto flight = row->get_value(flightColumns.flight);
    // managed works when used with attach ...
    auto pMenuPopup = Gtk::make_managed<Gtk::Menu>();
    auto pMenuitemIcao = Gtk::make_managed<Gtk::MenuItem>(flight->getIcao24());    // *pIcaoBtn
    auto uriIcao = Glib::ustring::sprintf("https://globe.adsbexchange.com/?icao=%s", flight->getIcao24());
    pMenuitemIcao->signal_activate().connect(
        sigc::bind(
            sigc::mem_fun(*this, &FlightsDlg::showLink)
        , uriIcao));
    pMenuPopup->append(*pMenuitemIcao);
    if (!flight->getCallsign().empty()) {
        auto uriCall = Glib::ustring::sprintf("https://www.flightaware.com/live/flight/%s", flight->getCallsign());
        auto pMenuitemCall = Gtk::make_managed<Gtk::MenuItem>(flight->getCallsign());
        pMenuPopup->append(*pMenuitemCall);
        pMenuitemCall->signal_activate().connect(
            sigc::bind(
                sigc::mem_fun(*this, &FlightsDlg::showLink)
            , uriCall));
    }
    pMenuPopup->show_all();
    pMenuPopup->attach_to_widget(*m_list);
    const GdkEvent* event = gtk_get_current_event();
    pMenuPopup->popup_at_pointer(event);
}

void
FlightsDlg::showLink(Glib::ustring& uri)
{
    try {
        m_starWin->show_uri(uri, gtk_get_current_event_time());
    }
    catch (Glib::Error& ex) {
        std::cout << "Error " << ex.what() << " open link " << uri << std::endl;
    }
}

void
FlightsDlg::show(StarWin* starWin)
{
    auto builder = Gtk::Builder::create();
    try {

        auto appl = starWin->getBackgroundAppl();
        builder->add_from_resource(appl->get_resource_base_path() + "/flights.ui");
        FlightsDlg* flightsDlg;
        builder->get_widget_derived("FlightsDlg", flightsDlg, starWin);
        flightsDlg->set_transient_for(*starWin);
        if (flightsDlg->run() == Gtk::RESPONSE_OK) {
        }
        else {
        }
        flightsDlg->hide();
        delete flightsDlg;
    }
    catch (const Glib::Error &ex) {
        std::cerr << "Unable to load flights.ui " << ex.what() << std::endl;
    }
}