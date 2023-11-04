/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2023 RPf <gpl3@pfeifer-syscon.de>
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

#include "StarDraw.hpp"
#include "HipparcosFormat.hpp"
#include "ConstellationFormat.hpp"
#include "Moon.hpp"
#include "Sun.hpp"

StarDraw::StarDraw(BaseObjectType* cobject
                  , const Glib::RefPtr<Gtk::Builder>& builder
                  , const Gtk::Application& appl)
: Gtk::DrawingArea(cobject)
{
    m_starFormat = std::make_shared<HipparcosFormat>(appl);
    m_starFormat->getStars();       // preinit
    m_constlFormat = std::make_shared<ConstellationFormat>(appl);
    m_constlFormat->getConstellations();
    setupConfig();
}

void
StarDraw::setupConfig()
{
    auto config = new Glib::KeyFile();
    std::string cfg = get_config_name();
    try {
        auto cfgFile = Gio::File::create_for_path(cfg);
        if (!cfgFile->query_exists()) {
            std::cerr << "Creating config " << cfg << " please set your location." << std::endl;
            config->set_double(GRP_MAIN, LATITUDE, m_geoPos.getLatDegrees());
            config->set_double(GRP_MAIN, LONGITUDE, m_geoPos.getLonDegrees());
            config->save_to_file(cfg);
        }
        else {
            if (config->load_from_file(cfg, Glib::KEY_FILE_NONE)
             && config->has_group(GRP_MAIN)) {
                if (config->has_key(GRP_MAIN, LATITUDE))
                    m_geoPos.setLatDegrees(config->get_double(GRP_MAIN, LATITUDE));
                if (config->has_key(GRP_MAIN, LONGITUDE))
                    m_geoPos.setLonDegrees(config->get_double(GRP_MAIN, LONGITUDE));
            }
        }
    }
    catch (const Glib::Error &ex) {
        std::cerr << "Error " << ex.what() << " loading " << cfg << std::endl;
    }
}

std::string
StarDraw::get_config_name()
{
    // share config with glglobe as we already have coordinates
    std::string fullPath = g_canonicalize_filename("glglobe.conf", Glib::get_user_config_dir().c_str());
    //std::cout << "using config " << fullPath << std::endl;
    return fullPath;
}

void
StarDraw::draw_sun(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const GeoPosition& geoPos, const Layout& layout)
{
    auto raDec = Sun::position(jd);
    //std::cout << "Sun ra " << raDec->getRaDegrees() << " dec " << raDec->getDecDegrees() << std::endl;
    auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
    //std::cout << "Sun az " << azAlt->getAzimutDegrees() << " az " << azAlt->getAltitudeDegrees() << std::endl;
    if (azAlt->isVisible()) {
        auto p = azAlt->toScreen(layout);
        ctx->set_source_rgb(0.8, 0.8, 0.3);
        ctx->arc(p.getX(), p.getY(), SUN_MOON_RADIUS, 0.0, M_PI * 2.0);
        ctx->fill();
    }
}

void
StarDraw::draw_moon(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const GeoPosition& geoPos, const Layout& layout)
{
    auto raDec = Moon::position(jd);
    auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
    if (azAlt->isVisible()) {
        Moon moon;
        ctx->save();
        auto p = azAlt->toScreen(layout);
        ctx->translate(p.getX(), p.getY());
        moon.showPhase(jd, ctx, SUN_MOON_RADIUS);
        ctx->restore();
    }
}


void
StarDraw::draw_stars(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const GeoPosition& geoPos, const Layout& layout)
{
    ctx->set_source_rgb(0.8, 0.8, 0.8);
    for (auto s : m_starFormat->getStars()) {
        auto raDec = s->getRaDec();
        auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
        if (azAlt->isVisible()) {
            auto p = azAlt->toScreen(layout);
            auto rs = std::max(3.0 - (s->getVmagnitude() / 2.0), 1.0);
            //std::cout << "x " << p.getX() << " y " << p.getY() << " rs " << rs << "\n";
            ctx->arc(p.getX(), p.getY(), rs, 0.0, M_PI * 2.0);
            ctx->fill();
        }
    }
}

void
StarDraw::draw_constl(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const GeoPosition& geoPos, const Layout& layout)
{
    ctx->set_font_size(10.0);
    for (auto c : m_constlFormat->getConstellations()) {
        Point2D sum;
        int count = 0;
        bool anyVisible = false;
        auto polylines = c->getPolylines();
        for (auto l : polylines) {
            int prio = l->getWidth();
            double gray = 0.8;
            if (prio <= 1) {
                ctx->set_line_width(2);
            }
            else {
                ctx->set_line_width(1);
                gray -= 0.2 * std::max(4 - prio, 1);
            }
            ctx->set_source_rgb(gray, gray, gray);
            bool visible = false;
            for (auto raDec : l->getPoints()) {
                auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
                if (azAlt->isVisible()) {
                    visible = true;
                }
            }
            if (visible) {
                anyVisible = true;
                for (auto raDec : l->getPoints()) {
                    auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
                    auto p = azAlt->toScreen(layout);
                    sum.add(p);
                    if (count == 0) {
                        ctx->move_to(p.getX(), p.getY());
                    }
                    else {
                        ctx->line_to(p.getX(), p.getY());
                    }
                    ++count;
                }
                ctx->stroke();
            }
        }
        if (anyVisible) {
            ctx->set_source_rgb(0.6, 0.6, 0.6);
            double avgX = sum.getX() / (double)count;
            double avgY = sum.getY() / (double)count;
            ctx->move_to(avgX, avgY);
            ctx->show_text(c->getName());
        }
    }
}

bool
StarDraw::on_draw(const Cairo::RefPtr<Cairo::Context>& ctx)
{
    int w = get_allocated_width();
    int height = get_allocated_height();
    //std::cout << "draw " << w << " h " << h << "\n";
    Layout layout(w, height);
    auto now = Glib::DateTime::create_now_utc();
    JulianDate jd(now);
    ctx->save();
    ctx->set_source_rgb(0.06, 0.06, 0.20);
    ctx->rectangle(0, 0, w, height);
    ctx->fill();
    ctx->translate((w/2), (height/2));
    double r = layout.getMin() / 2.0;
    ctx->arc(0.0, 0.0, r, 0.0, M_PI * 2.0);
    ctx->clip();    // as we draw some lines beyond horizon
    draw_constl(ctx, jd, m_geoPos, layout);
    draw_stars(ctx, jd, m_geoPos, layout);
    draw_moon(ctx, jd, m_geoPos, layout);
    draw_sun(ctx, jd, m_geoPos, layout);

    ctx->set_source_rgb(0.5, 0.5, 0.7);
    ctx->set_font_size(16.0);
    Cairo::FontExtents fontExtents;
    ctx->get_font_extents(fontExtents);
    ctx->move_to(0.0, r - 10.0);
	ctx->show_text("S");
    ctx->move_to(-r + 10.0, 0.0);
	ctx->show_text("E");
    ctx->move_to(0, -r + fontExtents.height);
	ctx->show_text("N");
    ctx->move_to(r - fontExtents.max_x_advance, 0.0);
	ctx->show_text("W");

    ctx->restore();
    return true;
}