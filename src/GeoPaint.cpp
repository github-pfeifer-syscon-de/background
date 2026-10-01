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
#include <Log.hpp>
#include <psc_i18n.hpp>
#include <WeatherConfig.hpp>
#include <OpenskyFlights.hpp>

#include "BackConfig.hpp"
#include "GeoPaint.hpp"
#include "StarWin.hpp"
#include "Math.hpp"

GeoPaint::GeoPaint(StarWin* starWin)
: BackPaint(starWin)
, m_geoConversion{std::make_shared<GeoConvLinear>()}
, m_geoMargin{m_config->getGeoMargin()}
{
    auto geoJson = m_config->getGeoJsonFile();
    setGeoJsonFile(geoJson);
    auto geoPointJson = m_config->getGeoPointsFile();
    setGeoPointsFile(geoPointJson);
    auto image = m_config->getImageFile();
    if (image.empty()) {    // set some default
        image = Glib::canonicalize_filename(DEFAULT_IMAGE , PACKAGE_DATA_DIR);
        m_config->setImageFile(image);
        m_config->save();
    }
    setImage(image);
    m_weatherTransparence = m_config->getWeatherTransparency();
    refresh_weather_service();
    refresh_flight_service(false);
}

std::string
GeoPaint::findFile(const std::string& name)
{
    auto file = m_starWin->getFileLoader()->findFile(name, SRC_DIR);;
    if (file) {
        return file->get_path();
    }
    // // try use given image config
    // if (m_config->hasKey(GROUP_GEO, KEY_IMAGE)) {
    //     auto image = Gio::File::create_for_path(m_config->getString(GROUP_GEO, KEY_IMAGE));
    //     auto imageDir = image->get_parent();
    //     auto file = imageDir->get_child(name);
    //     if (file->query_exists()) {
    //         return file->get_path();
    //     }
    //     return image->get_path();
    // }
    // // this will most likely fail
    // auto file = m_starWin->getFileLoader()->findFile(name);
    // if (file) {
    //     return file->get_path();
    // }
    std::cout << "GeoPaint::findFile name " << name << " was not found!" << std::endl;
    return "";
}

std::shared_ptr<WeatherConfig>
GeoPaint::get_config()
{
    return std::dynamic_pointer_cast<WeatherConfig>(m_starWin->getConfig());
}

void
GeoPaint::weather_transparency_changed(Gtk::Scale *scale)
{
    m_weatherTransparence = scale->get_value();
    m_starWin->update();
}

std::shared_ptr<Weather>
GeoPaint::get_weather()
{
    return m_weatherService;
}

std::shared_ptr<Weather>
GeoPaint::refresh_weather_service()
{
    auto conf =  m_config->getActiveWebMapServiceConf();
    if (conf) {
        m_weatherService = m_config->getService(this, conf);
        m_weatherService->setLog(m_starWin->getLog());
        //m_weatherService->setLog(m_log);
        m_weatherService->signal_products_completed().connect(
            sigc::mem_fun(*this, &GeoPaint::request_weather_product));
        m_weatherService->capabilities();
        psc::log::Log::logAdd(psc::log::Level::Debug, [&] {
             return std::format("requested weather capabilites {} ", conf->getName());
        });
    }
    return m_weatherService;
}

bool
GeoPaint::setGeoJsonFile(const std::string& geoJson)
{
    m_geoVectors = loadGeoFile(geoJson, "shape");
    return !m_geoVectors.empty();
}

bool
GeoPaint::setGeoPointsFile(const std::string& geoJson)
{
    m_geoPoints = loadGeoFile(geoJson, "points");
    return !m_geoPoints.empty();
}

std::vector<psc::geo::PtrFeature>
GeoPaint::loadGeoFile(const std::string& geoPointsFile, const std::string& ctx)
{
    std::vector<psc::geo::PtrFeature> geoFeatures;
    if (!geoPointsFile.empty()) {
        try {
            JsonHelper jsonHelper;
            jsonHelper.load_from_file(geoPointsFile);
            psc::geo::GeoJson2 geoJson2;
            geoFeatures = geoJson2.read(jsonHelper);
        }
        catch (const std::exception& exc) {
            auto msg = Glib::ustring::sprintf(_("Error %s processing %s %s"), exc.what(), geoPointsFile, ctx);
            m_starWin->showMessage(msg, Gtk::MESSAGE_WARNING);
        }
    }
    return geoFeatures;
}

bool
GeoPaint::setImage(const std::string& image)
{
    m_imagePix.reset();
    auto imagePath = image;
    if (!image.empty()) {
        auto imageFile = Gio::File::create_for_path(image);
        if (!imageFile->query_exists()) {     // not yet installed?
            // try lookup
            imageFile = m_starWin->getFileLoader()->findFile(imageFile->get_basename(), SRC_DIR);;
            imagePath = imageFile->get_path();
        }
        auto pixmap = Gdk::Pixbuf::create_from_file(imagePath);
        m_imagePix = std::make_shared<GeoBitmap>();
        m_imagePix->setPixmap(pixmap);
        m_imagePix->setMinimum(GeoCoordinate{-180.0, -90.0, COORD_REF});
        m_imagePix->setMaximum(GeoCoordinate{180.0, 90.0, COORD_REF});
    }
    return true;
}

void GeoPaint::setGeoMargin(double geoMargin)
{
    m_geoMargin = geoMargin;
    findGeoMinMax();    // this will just scale geo-display, weather will be updated on next refresh...
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
            m_weatherPix->setMinimum(m_min);
            m_weatherPix->setMaximum(m_max);
        }
        psc::log::Log::logAdd(psc::log::Level::Debug, [&] {
            return std::format("weather_image_notify bytes {} width {} height {}"
                , requestPixbuf->get_byte_length(), requestPixbuf->get_width(), requestPixbuf->get_height());;
        });
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
        // but maybe we should consider pix coords
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

void
GeoPaint::refresh()
{
    m_starWin->update();
}


int
GeoPaint::get_weather_image_size()
{
    return  m_config->getWeatherImageSize();
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
    auto weatherProductId = m_config->getWeatherProductId();
    if (!weatherProductId.empty() && m_weatherService) {
        m_weatherRequested = true;
        auto coordMin = m_geoConversion->fromDisplay(m_min);
        auto coordMax = m_geoConversion->fromDisplay(m_max);
        auto product = m_weatherService->find_product(weatherProductId);
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
            psc::log::Log::logAdd(psc::log::Level::Debug, [&] {
                return std::format("request_weather_product {} min {} max {}", weatherProductId
                , coordMin.toString(), coordMax.toString());;
            });
        }
        else {
            psc::log::Log::logAdd(psc::log::Level::Warn,
                "request_weather_product not expected product type");
        }
    }
}

bool
GeoPaint::findGeoMinMax()
{
    GeoCoordinate min{180.0,90.0, COORD_REF};
    GeoCoordinate max{-180.0,-90.0, COORD_REF};
    for (auto feature : m_geoPoints) { // also check points
        findGeoMinMax(feature->getGeometry(), min, max);
    }
    for (auto feature : m_geoVectors) {
        findGeoMinMax(feature->getGeometry(), min, max);
    }
    GeoCoordinate coordExt (m_geoMargin, m_geoMargin, COORD_REF);
    m_min = min.floor() - coordExt;
    m_max = max.ceil() + coordExt;
    auto diff = m_max - m_min;
    auto shortest = std::min(diff.getLongitude(), diff.getLatitude());
    m_max = m_min + GeoCoordinate(shortest, shortest, COORD_REF);   // shape long/lat equaly
    psc::log::Log::logAdd(psc::log::Level::Debug, [&] {
        return std::format("findGeoMinMax min {} max {}"
            , m_min.toString(), m_max.toString());;
    });
    return diff.getLatitude() > 0.0 && diff.getLongitude() > 0.0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winfinite-recursion"
void
GeoPaint::findGeoMinMax(const psc::geo::PtrGeometry& geom, GeoCoordinate& min, GeoCoordinate& max)
{
    auto pnt = std::dynamic_pointer_cast<psc::geo::Point>(geom);
    if (pnt != nullptr) {
        min.min(pnt->getCoordinate());
        max.max(pnt->getCoordinate());
    }
    auto segm = std::dynamic_pointer_cast<psc::geo::Segment>(geom);
    if (segm != nullptr) {
        for (const auto& coord : segm->getCoordinates()) {
            min.min(coord);
            max.max(coord);
        }
    }
    auto poly = std::dynamic_pointer_cast<psc::geo::Polygon>(geom);
    if (poly != nullptr) {
        for (auto segm : poly->getSegments()) {
            findGeoMinMax(segm, min, max);
        }
    }
    auto mult = std::dynamic_pointer_cast<psc::geo::MultiPolygon>(geom);
    if (mult != nullptr) {
        for (auto poly : mult->getPolygons()) {
            findGeoMinMax(poly, min, max);
        }
    }
}
#pragma GCC diagnostic pop

void
GeoPaint::drawFeatures(
           std::vector<psc::geo::PtrFeature>& features
        ,  Cairo::RefPtr<Cairo::Context>& ctx
        , double fact)
{
    ctx->set_line_width(0.7);
    ctx->set_antialias(Cairo::ANTIALIAS_DEFAULT);
    for (auto& feature : features) {
        auto geom = feature->getGeometry();
        drawGeometry(geom, feature, ctx, fact);
    }
}

void
GeoPaint::drawGeometry(
       const psc::geo::PtrGeometry& geom
     , const psc::geo::PtrFeature& feature
     , Cairo::RefPtr<Cairo::Context>& ctx
     , double fact)
{
    auto multiPoly = std::dynamic_pointer_cast<psc::geo::MultiPolygon>(geom);
    if (multiPoly) {
        for (auto& poly : multiPoly->getPolygons()) {
            drawGeometry(poly, feature, ctx, fact);
        }
    }
    auto poly = std::dynamic_pointer_cast<psc::geo::Polygon>(geom);
    if (poly) {
        for (auto& segm : poly->getSegments()) {
            drawGeometry(segm, feature, ctx, fact);
        }
    }
    auto segm = std::dynamic_pointer_cast<psc::geo::Segment>(geom);
    if (segm) {
        double red{1.0};
        double green{1.0};
        double blue{1.0};
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
        for (const auto& geoCoord : segm->getCoordinates()) {
            auto coord = m_geoConversion->toDisplay(geoCoord);
            auto xDraw = (coord.getLongitude() - m_min.getLongitude()) * fact;
            auto yDraw = (m_max.getLatitude() - coord.getLatitude()) * fact;    //  invert y as graphic coords are from top
            //std::cout << "x = " << x << " y = " << y
            //          << " xDraw = " << xDraw << " yDraw = " << yDraw << std::endl;
            if (firstInSegm) {
                ctx->move_to(xDraw, yDraw);
            }
            else {
                ctx->line_to(xDraw, yDraw);
            }
            firstInSegm = false;
        }
        ctx->stroke();
    }
    auto pnt = std::dynamic_pointer_cast<psc::geo::Point>(geom);
    if (pnt) {
        auto props = feature->getProperties();
        Glib::ustring name;
        if (props->getType("name") == psc::geo::ValueType::String) {
            name = props->getString("name");
        }
        int64_t population{};
        if (props->getType("population") == psc::geo::ValueType::Integer) {
            population = props->getInteger("population");
        }
        int64_t minPopulation = m_config->getPointsMinPopulation();
        if (population >= minPopulation) {
            ctx->set_source_rgb(0.2, 0.2, 0.8);
            auto coord = m_geoConversion->toDisplay(pnt->getCoordinate());
            auto xDraw = (coord.getLongitude() - m_min.getLongitude()) * fact;
            auto yDraw = (m_max.getLatitude() - coord.getLatitude()) * fact;    //  invert y as graphic coords are from top
            Cairo::TextExtents ext;
            ctx->get_text_extents(name, ext);
            ctx->move_to(xDraw - 3, yDraw - 3);
            ctx->line_to(xDraw + 3, yDraw + 3);
            ctx->move_to(xDraw + 3, yDraw - 3);
            ctx->line_to(xDraw - 3, yDraw + 3);
            ctx->stroke();
            if (!name.empty()) {
                ctx->move_to(xDraw - ext.width / 2.0, yDraw - ext.height);
                ctx->show_text(name);
            }
        }
    }

}

void
GeoPaint::drawGeoImage(
        Cairo::RefPtr<Cairo::Context>& ctx
      , GeoCoordinate& diff
      , int width, int height)
{
    if (m_imagePix) {
        auto coord = m_geoConversion->fromDisplay(m_min);
        auto target = m_imagePix->getSlice(coord, diff);
        auto scaled = target->scale_simple(width, height, Gdk::INTERP_BILINEAR);
        Gdk::Cairo::set_source_pixbuf(ctx, scaled, 0, 0);
        //ctx->rectangle(0, 0, width, height);  // strech result -> use progressive scale ?
        //ctx->fill();
        ctx->paint();
    }
}

void
GeoPaint::drawWeather(
        Cairo::RefPtr<Cairo::Context>& ctx
        , int width, int height)
{
    if (m_weatherService && !m_weatherRequested) {
        auto weatherProductId = m_config->getWeatherProductId();
        if (!weatherProductId.empty()) {
            auto prod = m_weatherService->find_product(weatherProductId);
            if (prod && !prod->is_latest()) {
                request_weather_product();
            }
        }
    }
    if (m_weatherPix) {
        // as this was rquested with the correct coords can use directly (but need to scale as we used a fixed request size)
        auto scaled = m_weatherPix->getPixmap()->scale_simple(width, height, Gdk::INTERP_BILINEAR);
        Gdk::Cairo::set_source_pixbuf(ctx, scaled, 0, 0);
        //ctx->rectangle(0, 0, width, height);
        //ctx->fill();
        ctx->paint_with_alpha(m_weatherTransparence);
    }
}

double
GeoPaint::heightToPixel(double height_m)
{
    auto pixel = height_m * 25.0 / 10000.0;   // 10000m -> 25pixel
    //std::cout << "GeoPaint::heightToPixel"
    //          << " h " << height_m<< "m"
    //          << " pix " << pixel << std::endl;
    return pixel;
}

void
GeoPaint::refresh_flight_service(bool force)
{
    auto service = m_config->getFlightService();
    if (service.empty()) {
        m_flights.clear();
        m_flightService.reset();
        return;
    }
    if (!m_flightService
     || service !=  m_flightService->getServiceName()) {
        m_flightService = Flights::getService(service);
        m_flightService->addListener(this);
        m_flightService->setUpdateInterval(
            std::chrono::minutes(
                m_config->getFlightRefreshMin()));
    }
    if (force || m_flightService->isUpdate()) {
        updateFlights();
    }
}

void
GeoPaint::updateFlights()
{
    // since service allows limited queries only give a small area
    GeoBounds flightBounds{
        m_config->getFlightLongitude() - m_config->getFlightBounds()
        , m_config->getFlightLatitude() - m_config->getFlightBounds()
        , m_config->getFlightLongitude() + m_config->getFlightBounds()
        , m_config->getFlightLatitude() + m_config->getFlightBounds()
        , COORD_REF};
    m_flightService->query(flightBounds);
}

std::shared_ptr<Flights>
GeoPaint::getFlightService()
{
    if (!m_flightService) { // if not yet done try to initialize
        refresh_flight_service(false);
    }
    return m_flightService;
}

void
GeoPaint::drawFlights(
        Cairo::RefPtr<Cairo::Context>& ctx
        , double fact)
{
    refresh_flight_service(false);
    ctx->set_line_width(1.0);
    ctx->set_antialias(Cairo::ANTIALIAS_DEFAULT);
    for (auto& flight : m_flights) {
        auto geoCoord = flight->getPosition();
        auto coord = m_geoConversion->toDisplay(geoCoord);
        auto xDraw = (coord.getLongitude() - m_min.getLongitude()) * fact;
        auto yDraw = (m_max.getLatitude() - coord.getLatitude()) * fact;    //  invert y as graphic coords are from top
        ctx->move_to(xDraw, yDraw);
        if (flight->isOnGround()) {
            ctx->set_source_rgb(0.3, 0.3, 0.3);
        }
        else {
            ctx->set_source_rgb(0.3, 1.0, 0.0);
            auto yTop = yDraw - heightToPixel(flight->getGeoAltitude());
            ctx->line_to(xDraw, yTop);
            ctx->stroke();
            ctx->move_to(xDraw, yTop);
        }
        auto name = flight->getCallsign();
        if (name.empty()) {
            name = flight->getIcao24();
        }
        ctx->show_text(name);
    }
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
    auto diff = m_max - m_min;
    if (std::abs(diff.getLongitude()) < 0.001
      ||std::abs(diff.getLatitude()) < 0.001) {
        return;
    }
    ctx->translate((layout.getWidth() - min) / 2, (layout.getHeight() - min) / 2);
    ctx->arc(min / 2, min / 2, min / 2, 0.0, Math::TWO_PI);
    ctx->clip();    // make it a round shape
    // since the result is quadratic adjust the difference as well
    drawGeoImage(ctx, diff, width, height);
    drawWeather(ctx, width, height);
    auto fact = static_cast<double>(width) / diff.getLongitude(); // since we use same long/latitude this should work
    drawFeatures(m_geoVectors, ctx, fact);
    drawFeatures(m_geoPoints, ctx, fact);
    drawFlights(ctx, fact);
    ctx->restore();
    //std::cout << "draw " << w << " h " << h << "\n";
    //queue_draw();
}

void GeoPaint::update(const std::vector<PtrFlight>& flights)
{
    psc::log::Log::logAdd(psc::log::Level::Info,  [&] {
        return std::format("Flights {} passed", flights.size());
    });
    m_flights = flights;
    refresh();
}

void GeoPaint::notifyError(const Glib::ustring& error, int status)
{
    psc::log::Log::logAdd(psc::log::Level::Error,
        std::format("Flights error {} status {}", error, status));
    std::cout << "GeoPaint::notifyError"
              << " error" << error
              << " status " << status << std::endl;
}

void
GeoPaint::closeConfigDlg()
{
    m_starWin->closeConfigDlg();
}

void
GeoPaint::save_config()
{
    m_starWin->saveConfig();
}

void
GeoPaint::on_action_preferences()   // reopen config
{
    m_starWin->onMenuConfig();
}
