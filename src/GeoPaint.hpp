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
#include <GeoJson.hpp>
#include <WebMapService.hpp>
#include <memory>
#include <WeatherConfigGrid.hpp>
#include <Flights.hpp>

#include "GeoConversion.hpp"
#include "GeoBitmap.hpp"
#include "BackPaint.hpp"

class StarWin;

class GeoPaint
: public BackPaint
, public WeatherConsumer
, public BaseConfigListener
, public FlightsConsumer {
public:
    GeoPaint(StarWin* starWin);
    explicit GeoPaint(const GeoPaint& other) = delete;
    virtual ~GeoPaint() = default;

    bool setGeoJsonFile(const std::string& geoJson);
    bool setImage(const std::string& image);
    void weather_image_notify(WeatherImageRequest& request) override;
    int get_weather_image_size() override;
    void drawImage(Cairo::RefPtr<Cairo::Context>&, const Glib::DateTime&, GeoPosition&, Layout&) override;
    static constexpr auto GROUP_GEO{"geo"};
    static constexpr auto KEY_GEOJSON{"geoJson"};
    static constexpr auto KEY_IMAGE{"image"};
    static constexpr auto DEFAULT_IMAGE{"2k_earth_daymap.jpg"};
    static constexpr auto SRC_DIR{"src"};
    static constexpr auto KEY_GEO_MARGIN{"geoMargin"};
    static constexpr auto KEY_DAYSTART{"dayStart"};
    static constexpr auto KEY_DAYEND{"dayEnd"};
    static constexpr auto GROUP_FLIGHTS{"flights"};
    static constexpr auto KEY_FLIGHT_SERVICE{"service"};
    static constexpr auto KEY_FLIGHT_REFRESH{"refresh"};
    static constexpr auto KEY_FLIGHT_LONGITUDE{"longitude"};
    static constexpr auto KEY_FLIGHT_LATITUDE{"latitude"};
    static constexpr auto KEY_FLIGHT_BOUNDS{"bounds"};

    std::string findFile(const std::string& name) override;
    std::shared_ptr<WeatherConfig> get_config() override;
    void weather_transparency_changed(Gtk::Scale *scale) override;
    std::shared_ptr<Weather> get_weather() override;
    std::shared_ptr<Weather> refresh_weather_service() override;
    void closeConfigDlg() override;
    void save_config() override;
    void on_action_preferences() override;
    void refresh();
    void setGeoMargin(double geoMargin);
    void update(std::list<PtrFlight> flights) override;
    void notifyError(const Glib::ustring& error, int status) override;

    // since we do no conversions at the moment use fixed reference system
    static constexpr auto COORD_REF{CoordRefSystem(CoordRefSystem::Value::EPSG_4326)};
    static constexpr auto USEC_MIN_INTERVAL{ 60l * G_USEC_PER_SEC};
    void refresh_flight_service(bool force);

protected:
    bool findGeoMinMax();
    void request_weather_product();
    void drawGeoImage(
         Cairo::RefPtr<Cairo::Context>& ctx
         , GeoCoordinate& diff
         , int width, int height);
    void drawWeather(
        Cairo::RefPtr<Cairo::Context>& ctx
        , int width, int height);
    void drawGeoShape(
         Cairo::RefPtr<Cairo::Context>& ctx
         , double fact);
    double heightToPixel(double height_m);
    void drawFlights(
        Cairo::RefPtr<Cairo::Context>& ctx
        , double fact);

private:
    GeoCoordinate m_min{};
    GeoCoordinate m_max{};
    GeoPath m_geoVector;
    std::shared_ptr<GeoBitmap> m_imagePix;
    std::shared_ptr<GeoConversion> m_geoConversion;
    std::shared_ptr<GeoBitmap> m_weatherPix;
    std::shared_ptr<Weather> m_weatherService;
    bool m_weatherRequested{};
    double m_weatherTransparence;
    double m_geoMargin{};
    std::shared_ptr<Flights> m_flightService;
    std::list<PtrFlight> m_flights;
};
