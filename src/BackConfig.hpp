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

#include <WeatherConfig.hpp>

class BackConfig
: public WeatherConfig
{
public:
    BackConfig(const char* confName);
    explicit BackConfig(const BackConfig& other) = delete;
    virtual ~BackConfig() = default;
    static constexpr auto DEFAULT_GEO_MARGIN{5.0};

    std::string getGeoJsonFile();
    void setGeoJsonFile(const std::string& geoJsonFile);
    std::string getImageFile();
    void setImageFile(const std::string& imageFile);
    double getGeoMargin();
    void setGeoMargin(double geoMargin);

    Glib::ustring getFlightService();
    void setFlightService(const Glib::ustring& service);
    int32_t getFlightRefreshMin();
    void setFlightRefreshMin(int32_t flightRefreshMin);
    double getFlightLongitude();
    void setFlightLongitude(double lon);
    double getFlightLatitude();
    void setFlightLatitude(double lat);
    double getFlightBounds();
    void setFlightBounds(double bounds);
    int getDayStart();
    void setDayStart(int dayStart);
    int getDayEnd();
    void setDayEnd(int dayEnd);

protected:
    std::string get_main_config_group() override;
};
