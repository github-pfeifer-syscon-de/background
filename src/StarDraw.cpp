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
#include <StringUtils.hpp>

#include "StarDraw.hpp"
#include "HipparcosFormat.hpp"
#include "ConstellationFormat.hpp"
#include "Moon.hpp"
#include "Sun.hpp"
#include "SysInfo.hpp"
#include "Math.hpp"
#include "BackgroundApp.hpp"
#include "FileLoader.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
StarDraw::StarDraw(BaseObjectType* cobject
                  , const Glib::RefPtr<Gtk::Builder>& builder
                  ,  const BackgroundApp& appl)
: Gtk::DrawingArea(cobject)
{
    auto fileLoader = std::make_shared<FileLoader>(appl.get_exec_path());
    m_starFormat = std::make_shared<HipparcosFormat>(fileLoader);
    //m_starFormat->getStars();       // preinit
    m_constlFormat = std::make_shared<ConstellationFormat>(fileLoader);
    //m_constlFormat->getConstellations();
    m_milkyway = std::make_shared<Milkyway>(fileLoader);
    setupConfig();
}
#pragma GCC diagnostic pop

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
    //Glib:canonicalize_file_name()
    //std::cout << "using config " << fullPath << std::endl;
    return fullPath;
}

void
StarDraw::drawInfo(const Cairo::RefPtr<Cairo::Context>& ctx, double size)
{
    ctx->set_source_rgb(TEXT_GRAY, TEXT_GRAY, TEXT_GRAY);
    ctx->set_font_size(size);
    Cairo::FontExtents fontExtents;
    ctx->get_font_extents(fontExtents);

    SysInfo sysInfo;
    int n = 2;
    for (auto info : sysInfo.allInfos()) {
        ctx->move_to(20.0, fontExtents.height * n++);
        ctx->show_text(info);
    }
}


// draw line from center outwards
static void drawRadialLine(const Cairo::RefPtr<Cairo::Context>& ctx, int value, int full, double inner, double outer) {
	double angleRad = 2.0 * M_PI * (double)value / (double)full;
	double xv = std::sin(angleRad);
	double yv = -std::cos(angleRad);
    ctx->move_to(inner * xv, inner * yv);
    ctx->line_to(outer * xv, outer * yv);
    ctx->stroke();
}

void
StarDraw::drawClock(const Cairo::RefPtr<Cairo::Context>& ctx, double radius)
{
    ctx->begin_new_path();  // as we get a strange stoke otherwise
    ctx->set_source_rgb(TEXT_GRAY, TEXT_GRAY, TEXT_GRAY);
    ctx->set_line_width(2.0);
    ctx->arc(0.0, 0.0, radius, 0, 2.0 * M_PI);
    ctx->stroke();
    Glib::DateTime dateTime = Glib::DateTime::create_now_local();
    drawRadialLine(ctx, dateTime.get_minute(), 60, 0.0, radius * 0.8);
    int hourM = (dateTime.get_hour() % 12) * 60 + dateTime.get_minute();
    drawRadialLine(ctx, hourM, 12 * 60, 0.0, radius * 0.6);
    ctx->set_line_width(1.0);
	const double inner15 = 0.9 * radius ;
	const double inner5 = 0.95 * radius;
	const double inner1 = 0.98 * radius;
	for (int i = 0; i < 60; ++i) {
	    double inner;
	    if (i % 15 == 0) {
            inner = inner15;
	    }
	    else if (i % 5 == 0) {
            inner = inner5;
	    }
	    else {
            inner = inner1;
	    }
	    drawRadialLine(ctx, i, 60, inner, radius);
	}
}

void
StarDraw::drawCalendar(const Cairo::RefPtr<Cairo::Context>& ctx, double size, const Layout& layout)
{
    double gray = TEXT_GRAY;
    ctx->set_source_rgb(gray, gray, gray);
    ctx->set_font_size(size);
    Cairo::FontExtents fontExtents;
    ctx->get_font_extents(fontExtents);
    ctx->translate(20.0, layout.getHeight() - 7.5 * fontExtents.height);
    const double colWidth = fontExtents.max_x_advance;
    int n = 0;
    Glib::DateTime dateNames = Glib::DateTime::create_utc(2024, 1, 1, 0, 0, 0); // start with monday
    for (auto wd = 1; wd <= 7; ++wd) {
        ctx->move_to(colWidth * n, 0);
        ctx->show_text(dateNames.format("%a"));
        dateNames = dateNames.add_days(1);
        ++n;
    }
    Glib::DateTime dateToday = Glib::DateTime::create_now_local();
    Glib::DateTime dateTime = Glib::DateTime::create_now_local();
    dateTime = dateTime.add_days(-(dateTime.get_day_of_month() - 1)); // beginning of month
    for (int row = 1; row < 7; ++row) {
        int wd = dateTime.get_day_of_week();
        for (int w = wd; w <= 7; ++w) {
            double gray = TEXT_GRAY;
            if (dateTime.get_day_of_month() == dateToday.get_day_of_month()) {
                gray = TEXT_GRAY_EMPHASIS;
            }
            ctx->set_source_rgb(gray, gray, gray);
            ctx->move_to(colWidth * (w-1), row * fontExtents.height);
            ctx->show_text(dateTime.format("%e"));
            dateTime = dateTime.add_days(1);
            if (dateTime.get_month() != dateToday.get_month()) {
                break;      // if month ended stop
            }
        }
        if (dateTime.get_month() != dateToday.get_month()) {
            break;      // if month ended stop
        }
    }
}


void
StarDraw::draw_planets(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const Layout& layout)
{
    ctx->save();
    ctx->set_font_size(10.0);
    Cairo::FontExtents fontExtents;
    ctx->get_font_extents(fontExtents);
	for (auto planet : planets) {
	    auto raDec = planet->getRaDecPositon(jd);
	    auto azAlt = m_geoPos.toAzimutAltitude(raDec, jd);
	    if (azAlt->isVisible()) {
            auto p = azAlt->toScreen(layout);
            ctx->arc(p.getX() - PLANET_READIUS, p.getY() - PLANET_READIUS, PLANET_READIUS, 0.0, 2.0*M_PI);
            ctx->set_source_rgb(TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS);
            ctx->fill();
            ctx->set_source_rgb(0.7, 0.7, 0.7);
            ctx->move_to(p.getX() + fontExtents.max_x_advance / 4.0,
                         p.getY() + fontExtents.height / 4.0);
            ctx->show_text(Glib::ustring::sprintf("%s %.1fAU", planet->getName(), raDec->getDistanceAU()));
	    }
	}
    ctx->restore();
}

void
StarDraw::draw_sun(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const Layout& layout)
{
    auto raDec = Sun::position(jd);
    //std::cout << "Sun ra " << raDec->getRaDegrees() << " dec " << raDec->getDecDegrees() << std::endl;
    auto azAlt = m_geoPos.toAzimutAltitude(raDec, jd);
    //std::cout << "Sun az " << azAlt->getAzimutDegrees() << " az " << azAlt->getAltitudeDegrees() << std::endl;
    if (azAlt->isVisible()) {
        auto p = azAlt->toScreen(layout);
        ctx->set_source_rgb(TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS, 0.3);
        ctx->arc(p.getX(), p.getY(), SUN_MOON_RADIUS, 0.0, M_PI * 2.0);
        ctx->fill();
    }
}

void
StarDraw::draw_moon(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const Layout& layout)
{
    auto raDec = Moon::position(jd);
    auto azAlt = m_geoPos.toAzimutAltitude(raDec, jd);
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
StarDraw::draw_milkyway(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const Layout& layout)
{
    for (auto poly : m_milkyway->getBounds()) {
        ctx->begin_new_path();
        int intens = poly->getIntensity();
        double dintens = 0.1 + (double)intens / 20.0;
        ctx->set_source_rgb(dintens, dintens, 0.25 + dintens);
        ctx->set_line_width(1.0);
        std::list<std::shared_ptr<AzimutAltitude>> azAlts;  // saving intermediate saves us recalculation
        bool anyVisible = false;
        for (auto raDec : poly->getPoints()) {
            auto azAlt = m_geoPos.toAzimutAltitude(raDec, jd);
            azAlts.push_back(azAlt);
            anyVisible |= azAlt->isVisible();
        }
        uint32_t skipped = 0;
        if (anyVisible) {       // do not draw if outside
            bool move = true;
            for (auto azAlt : azAlts) {
                if (azAlt->isVisible()) { // stop drawing beyond horizont, with some additional to allow closing
                    auto p = azAlt->toScreen(layout);
                    if (move) {
                        ctx->move_to(p.getX(), p.getY());
                        move = false;
                    }
                    else {
                        ctx->line_to(p.getX(), p.getY());
                    }
                }
                else {
                    move = true;
                    ++skipped;
                }
            }
        }
        else {
            skipped = static_cast<uint32_t>(azAlts.size());
        }
        //if (skipped > 0) {
        //    std::cout << __FILE__ << "::draw_milkyway"
        //              << " intens " << intens
        //              << " skipped " << skipped << "/" << azAlts.size() << std::endl;
        //}
        // the given data wraps nicely onto a sphere,
        //   but here we have a disc view (world),
        //    so we stick to some abstraction
        //ctx->close_path();
        //ctx->fill();
        ctx->stroke();
        auto raDec = m_milkyway->getGalacticCenter();
        auto azAlt = m_geoPos.toAzimutAltitude(raDec, jd);
        if (azAlt->isVisible()) {
            auto p = azAlt->toScreen(layout);
            auto w = static_cast<double>(layout.getMin()) / 200.0;
            ctx->move_to(p.getX()-w,p.getY());
            ctx->line_to(p.getX()+w,p.getY());
            ctx->move_to(p.getX(),p.getY()-w);
            ctx->line_to(p.getX(),p.getY()+w);
            ctx->set_source_rgb(0.6, 0.6, 0.6);
            ctx->set_line_width(1.0);
            ctx->stroke();
            ctx->move_to(p.getX()+w,p.getY()+w);
            ctx->show_text("Gal.cent.");
        }
    }
}

void
StarDraw::draw_stars(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const Layout& layout)
{
    ctx->set_source_rgb(TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS);
    for (auto s : m_starFormat->getStars()) {
        auto raDec = s->getRaDec();
        auto azAlt = m_geoPos.toAzimutAltitude(raDec, jd);
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
StarDraw::draw_constl(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const Layout& layout)
{
    ctx->set_font_size(10.0);
    for (auto c : m_constlFormat->getConstellations()) {
        Point2D sum;
        int count = 0;
        bool anyVisible = false;
        auto polylines = c->getPolylines();
        for (auto l : polylines) {
            int prio = l->getWidth();
            double gray = TEXT_GRAY_EMPHASIS;
            if (prio <= 1) {
                ctx->set_line_width(1.5);
            }
            else {
                ctx->set_line_width(1.0);
                gray -= 0.2 * std::max(4 - prio, 1);
            }
            ctx->set_source_rgb(gray, gray, gray);
            bool visible = false;
            for (auto raDec : l->getPoints()) {
                auto azAlt = m_geoPos.toAzimutAltitude(raDec, jd);
                if (azAlt->isVisible()) {
                    visible = true;
                }
            }
            if (visible) {
                anyVisible = true;
                for (auto raDec : l->getPoints()) {
                    auto azAlt = m_geoPos.toAzimutAltitude(raDec, jd);
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
            ctx->set_source_rgb(TEXT_GRAY, TEXT_GRAY, TEXT_GRAY);
            double avgX = sum.getX() / (double)count;
            double avgY = sum.getY() / (double)count;
            ctx->move_to(avgX, avgY);
            ctx->show_text(c->getName());
        }
    }
}

void
StarDraw::drawSky(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const Layout& layout)
{
    const double r = layout.getMin() / 2.0;
    auto grad = Cairo::RadialGradient::create((layout.getWidth()/2), (layout.getHeight()/2), r / 3.0, (layout.getWidth()/2), (layout.getHeight()/2), r);
    grad->add_color_stop_rgb(0.0, 0.06, 0.06, 0.15);
    grad->add_color_stop_rgb(1.0, 0.10, 0.10, 0.20);
    ctx->rectangle(0, 0, layout.getWidth(), layout.getHeight());
    ctx->set_source(grad);
    ctx->fill();
    ctx->translate((layout.getWidth()/2), (layout.getHeight()/2));
    ctx->arc(0.0, 0.0, r, 0.0, M_PI * 2.0);
    ctx->clip();    // as we draw some lines beyond horizon
    draw_milkyway(ctx, jd, layout);
    draw_constl(ctx, jd, layout);
    draw_stars(ctx, jd, layout);
    draw_moon(ctx, jd, layout);
    draw_sun(ctx, jd, layout);
    draw_planets(ctx, jd, layout);

    ctx->set_source_rgb(0.5, 0.5, TEXT_GRAY);
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
}


void
StarDraw::compute()
{
    int width = get_allocated_width();
    int height = get_allocated_height();
    if (!m_image
      || m_image->get_width() != width
      || m_image->get_height() != height) {
        m_image = Cairo::ImageSurface::create(Cairo::Format::FORMAT_ARGB32, width, height);
    }
    //m_image->flush(); // not needed? as we do it all by cairo!
    auto ctx = Cairo::Context::create(m_image);
    //auto start = g_get_monotonic_time();
    //std::cout << "draw " << w << " h " << h << "\n";
    Layout layout(width, height);
    auto now = Glib::DateTime::create_now_utc();
    JulianDate jd(now);
    ctx->save();
    drawSky(ctx, jd, layout);
    ctx->restore();
    ctx->save();
    drawInfo(ctx, 14.0);
    ctx->restore();
    //double cradius = 160.0;
    //ctx->save();  as we have this elsewhere disable for now
    //ctx->translate(20.0 + cradius, 160.0 + cradius);
    //drawClock(ctx, cradius);
    //ctx->restore();
    //ctx->save();
    //ctx->translate(20.0, 200.0); //  + cradius, layout.getHeight() / 2.0
    //drawNetInfo(ctx, cradius);
    //ctx->restore();
    ctx->save();
    drawCalendar(ctx, 16.0, layout);
    ctx->restore();
    //auto end = g_get_monotonic_time();
    //std::cout << "time to draw " << (end - start) << "us" << std::endl;
    // this is much faster than draw to screen directly, and is slightly faster than java
    // mark the image dirty so Cairo clears its caches.
    //m_image->mark_dirty();
}

bool
StarDraw::on_draw(const Cairo::RefPtr<Cairo::Context>& ctx)
{
    if (!m_image
      || m_image->get_width() != get_allocated_width()
      || m_image->get_height() != get_allocated_height()) {
        compute();
    }
    if (m_image) {
        ctx->set_source(m_image, 0, 0);
        ctx->paint();
    }
    return true;
}