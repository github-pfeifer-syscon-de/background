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

#include <memory>
#include <Flights.hpp>

#include "BackConfig.hpp"
#include "ConfigGridFlights.hpp"
#include "GeoPaint.hpp"

ConfigGridFlights::ConfigGridFlights(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, const std::shared_ptr<GeoPaint>& geoPaint)
: Gtk::Grid(cobject)
, m_geoPaint{geoPaint}
{
    auto config = std::dynamic_pointer_cast<BackConfig>(m_geoPaint->get_config());
    refBuilder->get_widget("comboService", m_comboService);
    m_comboService->append("", "");
    auto services = Flights::getServiceNames();
    for (auto service : services) {
        m_comboService->append(service, service);
    }
    m_comboService->set_active_id(config->getFlightService());
    //m_comboService->signal_changed().connect([&] {
    //   m_geoPaint->refresh_flight_service(true);
    //});
    refBuilder->get_widget("flightRefresh", m_flightRefresh);
    m_flightRefresh->set_value(config->getFlightRefreshMin());
    refBuilder->get_widget("flightLon", m_flightLon);
    m_flightLon->set_value(config->getFlightLongitude());
    refBuilder->get_widget("flightLat", m_flightLat);
    m_flightLat->set_value(config->getFlightLatitude());
    refBuilder->get_widget("flightBounds", m_flightBounds);
    m_flightBounds->set_value(config->getFlightBounds());

}

void
ConfigGridFlights::save()
{
    auto config = std::dynamic_pointer_cast<BackConfig>(m_geoPaint->get_config());
    config->setFlightService(m_comboService->get_active_id());
    config->setFlightRefreshMin(m_flightRefresh->get_value_as_int());
    config->setFlightLongitude(m_flightLon->get_value());
    config->setFlightLatitude(m_flightLat->get_value());
    config->setFlightBounds(m_flightBounds->get_value());
    m_geoPaint->refresh_flight_service(true);
}
