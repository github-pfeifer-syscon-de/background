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

#include "GeoConversion.hpp"
#include "GeoBitmap.hpp"
#include "BackPaint.hpp"

class StarWin;

class GeoPaint
: public BackPaint
, public WeatherConsumer
, public BaseConfigListener
{
public:
    GeoPaint(StarWin* starWin);
    explicit GeoPaint(const GeoPaint& other) = delete;
    virtual ~GeoPaint() = default;

    void setGeoJson(const std::string& geoJson);
    void setImage(const std::string& image);
     void weather_image_notify(WeatherImageRequest& request) override;
     int get_weather_image_size() override;
     void drawImage(Cairo::RefPtr<Cairo::Context>&, const Glib::DateTime&, GeoPosition&, Layout&) override;
    static constexpr auto GEO_BORDER{5.0};
    static constexpr auto GROUP_GEO{"geo"};
    static constexpr auto KEY_GEOJSON{"geoJson"};
    static constexpr auto KEY_IMAGE{"image"};
    static constexpr auto DEFAULT_IMAGE{"2k_earth_daymap.jpg"};
    static constexpr auto SRC_DIR{"src"};
    std::string  findFile(const std::string& name) override;
    std::shared_ptr<WeatherConfig> get_config() override;
    void weather_transparency_changed(Gtk::Scale *scale) override;
    std::shared_ptr<Weather> get_weather() override;
    std::shared_ptr<Weather> refresh_weather_service() override;
    void closeConfigDlg() override;
    void save_config() override;
    void on_action_preferences() override;

    // since we do no conversions at the moment use fixed reference system
    static constexpr auto COORD_REF{CoordRefSystem(CoordRefSystem::Value::EPSG_4326)};
protected:
    void findGeoMinMax();
    void request_weather_product();
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
};
