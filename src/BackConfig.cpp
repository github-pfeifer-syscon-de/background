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


#include "GeoPaint.hpp"
#include "BackConfig.hpp"


BackConfig::BackConfig(const char* confName)
: WeatherConfig(confName)
{
}

std::string
BackConfig::get_main_config_group()
{
    return GeoPaint::GROUP_GEO;
}

std::string
BackConfig::getGeoJsonFile()
{
    return getString(GeoPaint::GROUP_GEO, GeoPaint::KEY_GEOJSON);
}

void
BackConfig::setGeoJsonFile(const std::string& geoJsonFile)
{
    return setString(GeoPaint::GROUP_GEO, GeoPaint::KEY_GEOJSON, geoJsonFile);
}

std::string
BackConfig::getImageFile()
{
    return getString(GeoPaint::GROUP_GEO, GeoPaint::KEY_IMAGE);
}

void
BackConfig::setImageFile(const std::string& imageFile)
{
    return setString(GeoPaint::GROUP_GEO, GeoPaint::KEY_IMAGE, imageFile);
}

double
BackConfig::getGeoMargin()
{
    return getDouble(GeoPaint::GROUP_GEO, GeoPaint::KEY_GEO_MARGIN, DEFAULT_GEO_MARGIN);
}

void
BackConfig::setGeoMargin(double geoMargin)
{
    return setDouble(GeoPaint::GROUP_GEO, GeoPaint::KEY_GEO_MARGIN, geoMargin);
}

Glib::ustring
BackConfig::getFlightService()
{
    return getString(GeoPaint::GROUP_FLIGHTS, GeoPaint::KEY_FLIGHT_SERVICE);
}

void
BackConfig::setFlightService(const Glib::ustring& service)
{
    setString(GeoPaint::GROUP_FLIGHTS, GeoPaint::KEY_FLIGHT_SERVICE, service);
}


int32_t
BackConfig::getFlightRefreshMin()
{
    return getInteger(GeoPaint::GROUP_FLIGHTS, GeoPaint::KEY_FLIGHT_REFRESH, 15);
}

void
BackConfig::setFlightRefreshMin(int32_t flightRefreshMin)
{
    setInteger(GeoPaint::GROUP_FLIGHTS, GeoPaint::KEY_FLIGHT_REFRESH, flightRefreshMin);
}


double
BackConfig::getFlightLongitude()
{
    return getDouble(GeoPaint::GROUP_FLIGHTS, GeoPaint::KEY_FLIGHT_LONGITUDE);
}

void
BackConfig::setFlightLongitude(double lon)
{
    setDouble(GeoPaint::GROUP_FLIGHTS, GeoPaint::KEY_FLIGHT_LONGITUDE, lon);
}


double
BackConfig::getFlightLatitude()
{
    return getDouble(GeoPaint::GROUP_FLIGHTS, GeoPaint::KEY_FLIGHT_LATITUDE);
}

void
BackConfig::setFlightLatitude(double lat)
{
    setDouble(GeoPaint::GROUP_FLIGHTS, GeoPaint::KEY_FLIGHT_LATITUDE, lat);
}

double
BackConfig::getFlightBounds()
{
    return getDouble(GeoPaint::GROUP_FLIGHTS, GeoPaint::KEY_FLIGHT_BOUNDS);
}

void
BackConfig::setFlightBounds(double bounds)
{
    setDouble(GeoPaint::GROUP_FLIGHTS, GeoPaint::KEY_FLIGHT_BOUNDS, bounds);
}

int
BackConfig::getDayStart()
{
    return getInteger(GeoPaint::GROUP_GEO, GeoPaint::KEY_DAYSTART, 6);
}

void
BackConfig::setDayStart(int dayStart)
{
    setInteger(GeoPaint::GROUP_GEO, GeoPaint::KEY_DAYSTART, dayStart);
}

int
BackConfig::getDayEnd()
{
    return getInteger(GeoPaint::GROUP_GEO, GeoPaint::KEY_DAYEND, 18);
}

void
BackConfig::setDayEnd(int dayEnd)
{
    setInteger(GeoPaint::GROUP_GEO, GeoPaint::KEY_DAYEND, dayEnd);
}
