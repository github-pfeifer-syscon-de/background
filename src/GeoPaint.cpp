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

#include <iostream>

#include "GeoPaint.hpp"
#include "StarWin.hpp"
#include "Math.hpp"

GeoPaint::GeoPaint(StarWin* starWin)
: BackPaint(starWin)
, m_geoConversion{std::make_shared<GeoConvLinear>()}
{
    auto geoJson = m_config->getString(GROUP_GEO, KEY_GEOJSON);
    setGeoJson(geoJson);

    auto image = m_config->getString(GROUP_GEO, KEY_IMAGE);
    setImage(image);

    auto weatherProduct = m_config->getString(GROUP_GEO, KEY_WEATHER_PRODUCT);
    auto weatherAdress = m_config->getString(GROUP_WEATHER0, KEY_WEATHER_ADDRESS);
    auto weatherName = m_config->getString(GROUP_WEATHER0, KEY_WEATHER_NAME);
    auto weatherDelay =  m_config->getInteger(GROUP_WEATHER0, KEY_WEATHER_DELAY, 1800);
    auto weatherLocaltime = m_config->getBoolean(GROUP_WEATHER0, KEY_WEATHER_LOCALTIME, false);
    auto weatherType = m_config->getString(GROUP_WEATHER0, KEY_WEATHER_TYPE);
    auto conf = std::make_shared<WebMapServiceConf>(weatherName
        , weatherAdress
        , weatherDelay
        , weatherType
        , weatherLocaltime);
    setWeatherService(conf, weatherProduct);
}

void
GeoPaint::setGeoJson(const std::string& geoJson)
{
    m_geoVector.clear();
    if (!geoJson.empty()) {
        GeoJsonVectorHandler geoJsonVectorHandler;
        GeoJson geoJsonParse;
        geoJsonParse.read(geoJson, &geoJsonVectorHandler);
        m_geoVector = geoJsonVectorHandler.getPath();
        findGeoMinMax();
    }
}

void
GeoPaint::setImage(const std::string& image)
{
    m_imagePix.reset();
    if (!image.empty()) {
        auto pixmap = Gdk::Pixbuf::create_from_file(image);
        m_imagePix = std::make_shared<GeoBitmap>();
        m_imagePix->setPixmap(pixmap);
        m_imagePix->setMinimum(GeoCoord{-180.0, -90.0});
        m_imagePix->setMaximum(GeoCoord{180.0, 90.0});
    }
}

void
GeoPaint::weather_image_notify(WeatherImageRequest& request)
{
    auto requestPixbuf = request.get_pixbuf();
    m_weatherRequested = false;
    if (requestPixbuf) {
        if (!m_weatherPix) {
            m_weatherPix = std::make_shared<GeoBitmap>();
            //std::cout << "Requested weather capabilites weatherPix " << std::endl;
            m_weatherPix->setMinimum(GeoCoord{m_min});
            m_weatherPix->setMaximum(GeoCoord{m_max});
        }
        //std::cout << "GeoPaint::weather_image_notify weather "
        //          << requestPixbuf->get_width() << "x" << requestPixbuf->get_height() << std::endl;
        //auto weatherPixmap = m_weatherPix->getPixmap();
        //if (!weatherPixmap) {
        //    auto weather_width = get_weather_image_size();
        //    auto weather_height = get_weather_image_size();
        //    weatherPixmap = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, weather_width, weather_height);
            //std::cout << "create pixmap " << weatherPixmap->get_width() << "x" << weatherPixmap->get_height() << std::endl;
        //    m_weatherPix->setPixmap(weatherPixmap);
        //}
        // since we requested the correct area use it
        m_weatherPix->setPixmap(requestPixbuf);
        //request.mapping(requestPixbuf, weatherPixmap);
        m_starWin->update();
    }
    else {
        std::cout << "GeoPaint::weather_image_notify "
                  << " m_weatherPix " << static_cast<void*>(m_weatherPix.get())
                  << " pixbuf " <<  static_cast<void*>(requestPixbuf.get()) << std::endl;
    }
}

int
GeoPaint::get_weather_image_size()
{
    return 1024;
}

void
GeoPaint::request_weather_product()
{
    //auto weatherProductId = m_config->getWeatherProductId();
    //m_log->log(psc::log::Level::Info, [&] {
    //    return psc::fmt::format("request weather_product {}", weatherProductId);
    //});
    //m_weather_pix->fill(0x0);    // indicate something is going on by setting transp. black
    //update_weather_tex();
    if (!m_weatherProductId.empty() && m_weatherService) {
        m_weatherRequested = true;
        auto coordMin = m_geoConversion->fromDisplay(m_min);
        auto coordMax = m_geoConversion->fromDisplay(m_max);
        auto product = m_weatherService->find_product(m_weatherProductId);
        auto webMapProd = std::dynamic_pointer_cast<WebMapProduct>(product);
        if (webMapProd) {
            auto webMap = std::dynamic_pointer_cast<WebMapService>(m_weatherService);
            GeoBounds bounds(
                coordMin.getLongitude(), coordMin.getLatitude(),
                coordMax.getLongitude(), coordMax.getLatitude(),
                webMapProd->getCoordRefSystem());
            // does not consider 0 crossings -> eumet sat does not like these
            auto request = std::make_shared<WebMapImageRequest>(
                 webMap.get()
                , bounds
                , 0, 0
                , get_weather_image_size(), get_weather_image_size()
                , webMapProd);

            m_weatherService->requestImage(request);
        }
        else {
            std::cout << "GeoPaint::request_weather_product not expected product type !" << std::endl;
        }
    }
}

void
GeoPaint::setWeatherService(const std::shared_ptr<WebMapServiceConf>& conf, const std::string& weatherService)
{
    std::cout << "GeoPaint::setWeatherService" << std::endl;
    m_weatherProductId = weatherService;
    //m_weatherPix.reset();
    if (!weatherService.empty()) {
        m_weatherService = std::make_shared<WebMapService>(this, conf, 300);    // period sec
        //m_weatherService->setLog(m_log);
        m_weatherService->signal_products_completed().connect(
            sigc::mem_fun(*this, &GeoPaint::request_weather_product));
        m_weatherService->capabilities();
        std::cout << "GeoPaint::setWeatherService requested weather capabilites" << std::endl;
        // call as default request_weather_product
    }
}

void
GeoPaint::findGeoMinMax()
{
    GeoCoord min{180.0,90.0}, max{-180.0,-90.0};
    for (const auto& segm : m_geoVector) {
        bool firstPnt{true};
        GeoCoord geoCoord;
        for (auto& pnt : *segm) {
            geoCoord.set(firstPnt, pnt);
            if (!firstPnt) {
                min.min(geoCoord);
                max.max(geoCoord);
            }
            firstPnt = !firstPnt;
        }
    }
    m_min = min.floor() - GeoCoord(GEO_BORDER);
    m_max = max.ceil() + GeoCoord(GEO_BORDER);
    auto diff = m_max - m_min;
    auto shortest = std::min(diff.getLongitude(), diff.getLatitude());
    m_max = m_min + GeoCoord{shortest};   // shape long/lat equaliy
}

void
GeoPaint::drawImage(
          Cairo::RefPtr<Cairo::Context>& ctx
        , const Glib::DateTime& dateTime
        , GeoPosition& geoPosition
        , Layout& layout)
{
    auto min = std::min(layout.getWidth(), layout.getHeight());
    auto width = min;
    auto height = min;
    ctx->save();
    ctx->set_source_rgb(0.1, 0.1,  0.1);
    ctx->rectangle(0, 0, layout.getWidth(), layout.getHeight());
    ctx->fill();

    //std::cout << "GeoPaint::update"
    //          << " width " << width<< " min " << m_min.longitude << " max " << m_max.longitude << "\n"
    //          << " height " << height << " min " << m_min.latitude << " max " << m_max.latitude << std::endl;
    auto diff = m_max - m_min;
    if (std::abs(diff.getLongitude()) < 0.001
      ||std::abs(diff.getLatitude()) < 0.001) {
        return;
    }
    ctx->translate((layout.getWidth() - min) / 2, (layout.getHeight() - min) / 2);
    ctx->arc(min / 2, min / 2, min / 2, 0.0, Math::TWO_PI);
    ctx->clip();    // make it a round shape
    // since the result is quadratic adjust the difference as well
    auto fact = static_cast<double>(width) / diff.getLongitude(); // since we use same long/latitude this should work
    if (m_imagePix) {
        GeoCoord geoCoord{m_min.getLongitude(), m_min.getLatitude()};
        auto coord = m_geoConversion->fromDisplay(geoCoord);
        auto target = m_imagePix->getSlice(coord, diff);
        auto scaled = target->scale_simple(width, height, Gdk::INTERP_BILINEAR);
        Gdk::Cairo::set_source_pixbuf(ctx, scaled, 0, 0);
        //ctx->rectangle(0, 0, width, height);  // strech result -> use progressive scale ?
        //ctx->fill();
        ctx->paint();
    }
    if (m_weatherService && !m_weatherRequested) {
        auto prod = m_weatherService->find_product(m_weatherProductId);
        if (prod && !prod->is_latest()) {
            request_weather_product();
        }
    }
    if (m_weatherPix) {
        // as this was rquested with the correct coords can use directly (but need to scale as we used a fixed request size)
        auto scaled = m_weatherPix->getPixmap()->scale_simple(width, height, Gdk::INTERP_BILINEAR);
        Gdk::Cairo::set_source_pixbuf(ctx, scaled, 0, 0);
        //ctx->rectangle(0, 0, width, height);
        //ctx->fill();
        ctx->paint_with_alpha(0.6);
    }
    bool activePath{false};
    double red{1.0};
    double green{1.0};
    double blue{1.0};
    ctx->set_line_width(0.7);
    ctx->set_antialias(Cairo::ANTIALIAS_DEFAULT);
    for (const auto& segm : m_geoVector) {
        //red += 0.1;
        //if (red > 1.0) {
        //    red = 0.5;
        //}
        //green -= 0.1;
        //if (green < 0.5) {
        //    green = 1.0;
        //    blue += 0.1;
        //    if (blue > 1.0) {
        //        blue = 0.5;
        //    }
        //}
        ctx->set_source_rgb(red, green, blue);
        bool firstInSegm{true};
        bool firstPnt{true};
        GeoCoord geoCoord;
        for (auto& pnt : *segm) {
            geoCoord.set(firstPnt, pnt);
            if (!firstPnt) {
                auto coord = m_geoConversion->toDisplay(geoCoord);
                auto xDraw = (coord.getLongitude() - m_min.getLongitude()) * fact;
                auto yDraw = (m_max.getLatitude() - coord.getLatitude()) * fact;    //  invert y as graphic coords are from top
                //std::cout << "x = " << x << " y = " << y
                //          << " xDraw = " << xDraw << " yDraw = " << yDraw << std::endl;
                if (firstInSegm) {
                    if (activePath) {
                        ctx->stroke();
                        activePath = false;
                    }
                    ctx->move_to(xDraw, yDraw);
                }
                else {
                    ctx->line_to(xDraw, yDraw);
                    activePath = true;
                }
                firstInSegm = false;
            }
            firstPnt = !firstPnt;
        }
    }
    ctx->restore();
    //std::cout << "draw " << w << " h " << h << "\n";
    //queue_draw();
}

