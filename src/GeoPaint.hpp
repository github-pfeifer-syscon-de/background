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

#include "GeoConversion.hpp"
#include "GeoBitmap.hpp"
#include "BackPaint.hpp"

class StarWin;

class GeoPaint
: public BackPaint
, public WeatherConsumer
{
public:
    GeoPaint(StarWin* starWin);
    explicit GeoPaint(const GeoPaint& other) = delete;
    virtual ~GeoPaint() = default;

    void setGeoJson(const std::string& geoJson);
    void setImage(const std::string& image);
    void setWeatherService(
          const std::shared_ptr<WebMapServiceConf>& conf
        , const std::string& WeatherService);
     void weather_image_notify(WeatherImageRequest& request) override;
     int get_weather_image_size() override;
     void drawImage(Cairo::RefPtr<Cairo::Context>&, const Glib::DateTime&, GeoPosition&, Layout&) override;
    static constexpr auto GEO_BORDER{5.0};
    static constexpr auto GROUP_GEO{"geo"};
    static constexpr auto KEY_GEOJSON{"geoJson"};
    static constexpr auto KEY_IMAGE{"image"};
    static constexpr auto KEY_WEATHER_PRODUCT{"weatherProduct"};
    static constexpr auto GROUP_WEATHER0{"weather0"};
    static constexpr auto KEY_WEATHER_ADDRESS{"weatherAddress"};
    static constexpr auto KEY_WEATHER_NAME{"weatherName"};
    static constexpr auto KEY_WEATHER_DELAY{"weatherDelay"};
    static constexpr auto KEY_WEATHER_LOCALTIME{"weatherLocalTime"};
    static constexpr auto KEY_WEATHER_TYPE{"weatherType"};

protected:
    void findGeoMinMax();
    void request_weather_product();
private:
    GeoCoord m_min{};
    GeoCoord m_max{};
    GeoPath m_geoVector;
    std::shared_ptr<GeoBitmap> m_imagePix;
    std::shared_ptr<GeoConversion> m_geoConversion;
    std::shared_ptr<GeoBitmap> m_weatherPix;
    std::shared_ptr<Weather> m_weatherService;
    Glib::ustring m_weatherProductId;
    bool m_weatherRequested{};
};
