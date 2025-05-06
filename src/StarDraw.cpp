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
#include "ParamDlg.hpp"
#include "StarWin.hpp"
#include "config.h"
#include "Planets.hpp"


void
Grid::put(Glib::RefPtr<Pango::Layout>& layout
           , const Cairo::RefPtr<Cairo::Context>& ctx
           , int col, int row
           , double halign, int colSpan
           , double valign, int rowSpan)
{
    // the pango layout alignment did not work as expected with with
    // to make text look nicely put it on exact pixel location
    int x = col * m_cellWidth;
    int y = row * m_cellHeight;
    int textWidth, textHeight;
    layout->get_pixel_size(textWidth, textHeight);
    int baseline = layout->get_baseline();
    std::cout << " t " << layout->get_text()
              << " w " << textWidth
              << " h " << textHeight
              << " b " << baseline << std::endl;
    int cellWidth = colSpan * m_cellWidth;
    int cellHeight = rowSpan * m_cellHeight;
    // unsure what the exact conversion from pango base line to pixel is 1024 seems to fit here
    ctx->move_to(static_cast<int>(x + (cellWidth - textWidth) * halign)
               , static_cast<int>(y + (cellHeight - baseline / 1024) * valign));
    layout->show_in_cairo_context(ctx);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
StarDraw::StarDraw(BaseObjectType* cobject
                  , const Glib::RefPtr<Gtk::Builder>& builder
                  , StarWin* starWin)
: Gtk::DrawingArea(cobject)
, m_starWin{starWin}
{
    auto fileLoader = std::make_shared<FileLoader>(m_starWin->getBackgroundAppl()->get_exec_path());
    m_starFormat = std::make_shared<HipparcosFormat>(fileLoader);
    //m_starFormat->getStars();       // preinit
    m_constlFormat = std::make_shared<ConstellationFormat>(fileLoader);
    //m_constlFormat->getConstellations();
    m_milkyway = std::make_shared<Milkyway>(fileLoader);
	add_events(Gdk::EventMask::BUTTON_PRESS_MASK);
    setupConfig();
}
#pragma GCC diagnostic pop

void
StarDraw::setupConfig()
{
    m_config = std::make_shared<KeyConfig>("background.conf");
    std::string cfg = getGlobeConfigName();
    // since it is more convenient to use the location we saved for glglobe load&save it from there
    //   but this keepts the risk of overwriting the coordinates (if you change them on both sides...)
    try {
        auto cfgFile = Gio::File::create_for_path(cfg);
        if (!cfgFile->query_exists()) {
            Glib::ustring msg("No config found, please enter your position.");
            Gtk::MessageDialog dlg(msg);
            dlg.run();
            on_menu_param();
        }
        else {
            auto config = std::make_shared<Glib::KeyFile>();
            if (config->load_from_file(cfg, Glib::KEY_FILE_NONE)
             && config->has_group(GRP_GLGLOBE_MAIN)) {
                if (config->has_key(GRP_GLGLOBE_MAIN, LATITUDE_KEY))
                    m_geoPos.setLatDegrees(config->get_double(GRP_GLGLOBE_MAIN, LATITUDE_KEY));
                if (config->has_key(GRP_GLGLOBE_MAIN, LONGITUDE_KEY))
                    m_geoPos.setLonDegrees(config->get_double(GRP_GLGLOBE_MAIN, LONGITUDE_KEY));
            }
        }
    }
    catch (const Glib::Error &ex) {
        auto msg = Glib::ustring::sprintf("Error %s loading %s", ex.what(), cfg);
        Gtk::MessageDialog dlg(msg, false, Gtk::MessageType::MESSAGE_ERROR);
        dlg.run();
    }
}

std::string
StarDraw::getGlobeConfigName()
{
    // share config with glglobe as we already have coordinates
    std::string fullPath = g_canonicalize_filename("glglobe.conf", Glib::get_user_config_dir().c_str());
    //Glib:canonicalize_file_name()
    //std::cout << "using config " << fullPath << std::endl;
    return fullPath;
}

void
StarDraw::drawInfo(const Cairo::RefPtr<Cairo::Context>& ctx)
{
    getInfoColor(ctx);
    auto infoFont = getInfoFont();
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(infoFont);
    SysInfo sysInfo;
    int width, height{0};
    std::string text;
    text.reserve(512);
    for (auto info : sysInfo.allInfos()) {
        text += info + "\n";
        if (height == 0) {
            pangoLayout->set_text(info);
            pangoLayout->get_pixel_size(width, height);
        }
    }
    pangoLayout->set_text(text);
    ctx->move_to(20.0, height * 2);
    pangoLayout->show_in_cairo_context(ctx);
}


// draw line from center outwards
static void
drawRadialLine(const Cairo::RefPtr<Cairo::Context>& ctx, int value, int full, double inner, double outer) {
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
StarDraw::drawCalendar(const Cairo::RefPtr<Cairo::Context>& ctx, const Layout& layout)
{
    // since there seems no way to diffrentiate the locale
    //  dependent we use the Glib::DateTime default (monday)
    getCalendarColor(ctx);
    auto calFont = getCalendarFont();
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(calFont);
    scale(calFont, 0.6);
    auto smallLayout = Pango::Layout::create(ctx);
    smallLayout->set_font_description(calFont);
    int width, height;
    pangoLayout->set_text("M"); // use em as reference
    pangoLayout->get_pixel_size(width, height);
    ctx->translate(30.0, layout.getHeight() - 8.5 * height);
    Grid grid{static_cast<int>(width * 2.5), height};
    Glib::DateTime dateToday = Glib::DateTime::create_now_local();
    pangoLayout->set_text(dateToday.format("%B"));  // month name
    grid.put(pangoLayout, ctx, 1, 0, 0.5, 7);
    Glib::DateTime dateNames = Glib::DateTime::create_utc(2024, 1, 1, 0, 0, 0); // start with a monday
    for (auto wd = 1; wd <= 7; ++wd) {
        pangoLayout->set_text(dateNames.format("%a"));  // weekday abbr.
        grid.put(pangoLayout, ctx, wd, 1, 1.0);
        dateNames = dateNames.add_days(1);
    }
    Glib::DateTime dateTime = Glib::DateTime::create_now_local();
    dateTime = dateTime.add_days(-(dateTime.get_day_of_month() - 1)); // beginning of month
    for (int row = 2; row < 8; ++row) {
        smallLayout->set_text(dateTime.format("%V"));
        grid.put(smallLayout, ctx, 0, row, 1.0);
        int wd = dateTime.get_day_of_week();
        for (int w = wd; w <= 7; ++w) {
            Gdk::RGBA calColor = getCalendarColor();
            if (dateTime.get_day_of_month() == dateToday.get_day_of_month()) {
                brighten(calColor, 1.3);
            }
            ctx->set_source_rgb(calColor.get_red(), calColor.get_green(), calColor.get_blue());
            pangoLayout->set_text(dateTime.format("%e"));
            grid.put(pangoLayout, ctx, w, row, 1.0);
            dateTime = dateTime.add_days(1);
            if (dateTime.get_month() != dateToday.get_month()) {
                return;      // if month ended stop
            }
        }
    }
}


void
StarDraw::draw_planets(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
{
    ctx->save();
    auto starDesc = getStarFont();
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(starDesc);
    int width, height;
    Planets planets;
	for (auto& planet : planets.getOtherPlanets()) {
#       ifdef DEBUG
        std::cout << "StarDraw::draw_planets " << planet->getName() << std::endl;
#       endif
	    auto raDec = planet->getRaDecPositon(jd);
	    auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
	    if (azAlt->isVisible()) {
            auto p = azAlt->toScreen(layout);
            ctx->arc(p.getX() - PLANET_RADIUS, p.getY() - PLANET_RADIUS, PLANET_RADIUS, 0.0, 2.0*M_PI);
            ctx->set_source_rgb(TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS);
            ctx->fill();
            ctx->set_source_rgb(TEXT_GRAY_MID, TEXT_GRAY_MID, TEXT_GRAY_MID);
            pangoLayout->set_text(Glib::ustring::sprintf("%s %.1fAU", planet->getName(), raDec->getDistanceAU()));
            pangoLayout->get_pixel_size(width, height);
            ctx->move_to( p.getX() + 4.0
                        , p.getY() - static_cast<double>(height) / 2.0);   //
            pangoLayout->show_in_cairo_context(ctx);
	    }
	}
    ctx->restore();
}

void
StarDraw::draw_sun(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
{
    auto raDec = Sun::position(jd);
    //std::cout << "Sun ra " << raDec->getRaDegrees() << " dec " << raDec->getDecDegrees() << std::endl;
    auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
    //std::cout << "Sun az " << azAlt->getAzimutDegrees() << " az " << azAlt->getAltitudeDegrees() << std::endl;
    if (azAlt->isVisible()) {
        auto p = azAlt->toScreen(layout);
        ctx->set_source_rgb(TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS, TEXT_GRAY_LOW);
        ctx->arc(p.getX(), p.getY(), SUN_MOON_RADIUS, 0.0, M_PI * 2.0);
        ctx->fill();
    }
}

void
StarDraw::draw_moon(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
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
StarDraw::draw_milkyway(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
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
            auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
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
        auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
        if (azAlt->isVisible()) {
            ctx->set_source_rgb(TEXT_GRAY, TEXT_GRAY, TEXT_GRAY);
            auto p = azAlt->toScreen(layout);
            auto w = static_cast<double>(layout.getMin()) / 200.0;
            ctx->move_to(p.getX()-w,p.getY());
            ctx->line_to(p.getX()+w,p.getY());
            ctx->move_to(p.getX(),p.getY()-w);
            ctx->line_to(p.getX(),p.getY()+w);
            ctx->set_line_width(1.0);
            ctx->stroke();
            ctx->move_to(p.getX()+w,p.getY()+w);
            auto starDesc = getStarFont();
            auto pangoLayout = Pango::Layout::create(ctx);
            pangoLayout->set_font_description(starDesc);
            pangoLayout->set_text("Gal.cent.");
            pangoLayout->show_in_cairo_context(ctx);

        }
    }
}

void
StarDraw::draw_stars(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
{
    ctx->set_source_rgb(TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS, TEXT_GRAY_EMPHASIS);
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
StarDraw::draw_constl(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
{
    auto starDesc = getStarFont();
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(starDesc);
    int width, height;
    pangoLayout->set_text("M");
    pangoLayout->get_pixel_size(width, height);
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
            ctx->set_source_rgb(TEXT_GRAY, TEXT_GRAY, TEXT_GRAY);
            double avgX = sum.getX() / (double)count;
            double avgY = sum.getY() / (double)count;
            ctx->move_to(avgX, avgY);
            pangoLayout->set_text(c->getName());
            pangoLayout->show_in_cairo_context(ctx);
        }
    }
}

void
StarDraw::drawSky(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout)
{
    const double r = layout.getMin() / 2.0;
    auto grad = Cairo::RadialGradient::create((layout.getWidth()/2), (layout.getHeight()/2), r / 3.0, (layout.getWidth()/2), (layout.getHeight()/2), r);
    auto startColor = getStartColor();
    auto stopColor = getStopColor();
    grad->add_color_stop_rgb(0.0, startColor.get_red(), startColor.get_green(), startColor.get_blue());
    grad->add_color_stop_rgb(1.0, stopColor.get_red(), stopColor.get_green(), stopColor.get_blue());
    ctx->rectangle(0, 0, layout.getWidth(), layout.getHeight());
    ctx->set_source(grad);
    ctx->fill();
    ctx->translate((layout.getWidth()/2), (layout.getHeight()/2));
    ctx->arc(0.0, 0.0, r, 0.0, M_PI * 2.0);
    ctx->clip();    // as we draw some lines beyond the horizon
    draw_milkyway(ctx, jd, geoPos, layout);
    draw_constl(ctx, jd, geoPos, layout);
    draw_stars(ctx, jd, geoPos, layout);
    draw_moon(ctx, jd, geoPos, layout);
    draw_sun(ctx, jd, geoPos, layout);
    draw_planets(ctx, jd, geoPos, layout);

    ctx->set_source_rgb(TEXT_GRAY, TEXT_GRAY, TEXT_GRAY);
    auto starFont = getStarFont();
    scale(starFont, 1.75);
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(starFont);
    pangoLayout->set_text("S");
    int width, height;
    pangoLayout->get_pixel_size(width, height);
    ctx->move_to(0.0, r - height);
    pangoLayout->show_in_cairo_context(ctx);
    pangoLayout->set_text("E");
    pangoLayout->get_pixel_size(width, height);
    ctx->move_to(-r + width, 0.0);
    pangoLayout->show_in_cairo_context(ctx);
    pangoLayout->set_text("N");
    pangoLayout->get_pixel_size(width, height);
    ctx->move_to(0.0, -r + height);
	pangoLayout->show_in_cairo_context(ctx);
    pangoLayout->set_text("W");
    pangoLayout->get_pixel_size(width, height);
    ctx->move_to(r - width, 0.0);
	pangoLayout->show_in_cairo_context(ctx);
}


void
StarDraw::compute()
{
    if (!m_updateBlocked) { // no default updating while dialog is showing
        auto now = Glib::DateTime::create_now_utc();
        update(now, m_geoPos);
    }
}


void
StarDraw::update(Glib::DateTime now, GeoPosition& pos)
{
#   ifdef DEBUG
    std::cout << "StarDraw::update"
              << " date " << now.format_iso8601()
              << " pos " << pos.getLatDegrees() << " " << pos.getLonDegrees()
              << std::endl;
#   endif
    m_displayTimeUtc = now;
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
    JulianDate jd(now);
    //std::cout << std::fixed << "jd " << jd.getJulianDate() << std::endl;
    ctx->save();
    drawSky(ctx, jd, pos, layout);
    ctx->restore();
    ctx->save();
    drawInfo(ctx);
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
    drawCalendar(ctx, layout);
    ctx->restore();
    //auto end = g_get_monotonic_time();
    //std::cout << "time to draw " << (end - start) << "us" << std::endl;
    // this is much faster than draw to screen directly, and is slightly faster than java
    // mark the image dirty so Cairo clears its caches.
    //m_image->mark_dirty();
    queue_draw();
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

bool
StarDraw::on_button_press_event(GdkEventButton* event)
{
    if (event->button == 3) {
		Gtk::Menu* popupMenu = build_popup();
		// deactivate prevent item signals to get generated ...
		// signal_unrealize will never get generated
		popupMenu->attach_to_widget(*this); // this does the trick and calls the destructor
		popupMenu->popup(event->button, event->time);

		return true; // It has been handled.
	}
	return false;
}

Gtk::Menu *
StarDraw::build_popup()
{
	// managed works when used with attach ...
	auto pMenuPopup = Gtk::make_managed<Gtk::Menu>();
	auto mparam = Gtk::make_managed<Gtk::MenuItem>("_Parameter", true);
	mparam->signal_activate().connect(sigc::mem_fun(*this, &StarDraw::on_menu_param));
	pMenuPopup->append(*mparam);

	auto mabout = Gtk::make_managed<Gtk::MenuItem>("_About", true);
	mabout->signal_activate().connect(sigc::mem_fun(m_starWin->getBackgroundAppl(), &BackgroundApp::on_action_about));
	pMenuPopup->append(*mabout);



	pMenuPopup->show_all();
	return pMenuPopup;
}

void
StarDraw::on_menu_param()
{
    m_updateBlocked = true;
	ParamDlg::show(this);
    m_updateBlocked = false;
    compute();      // reset to default view
}

void
StarDraw::scale(Pango::FontDescription& starFont, double scale)
{
    starFont.set_size(static_cast<int>(starFont.get_size() * scale));
}

void StarDraw::brighten(Gdk::RGBA& calColor, double factor)
{
    calColor.set_red(calColor.get_red() * factor);
    calColor.set_green(calColor.get_green() * factor);
    calColor.set_blue(calColor.get_blue() * factor);
}

GeoPosition
StarDraw::getGeoPosition()
{
    return m_geoPos;
}

void
StarDraw::setGeoPosition(const GeoPosition& geoPos)
{
    m_geoPos = geoPos;
}

void
StarDraw::saveConfig()
{
    // handle glglobe&background config
    std::string cfg = getGlobeConfigName();
    try {
        m_config->saveConfig();
        auto config = std::make_shared<Glib::KeyFile>();
        auto cfgFile = Gio::File::create_for_path(cfg);
        if (cfgFile->query_exists()) {     // do load as file may contains other stuff
            config->load_from_file(cfg, Glib::KEY_FILE_NONE);
        }
        config->set_double(GRP_GLGLOBE_MAIN, LATITUDE_KEY, m_geoPos.getLatDegrees());
        config->set_double(GRP_GLGLOBE_MAIN, LONGITUDE_KEY, m_geoPos.getLonDegrees());
        config->save_to_file(cfg);
    }
    catch (const Glib::Error &ex) {
        auto msg = Glib::ustring::sprintf("Error %s saving %s", ex.what(), cfg);
        Gtk::MessageDialog dlg(msg, false, Gtk::MessageType::MESSAGE_ERROR);
        dlg.run();
    }
}

std::shared_ptr<KeyConfig>
StarDraw::getConfig()
{
    return m_config;
}

StarWin* StarDraw::getWindow()
{
    return m_starWin;
}

int
StarDraw::getIntervalMinutes()
{
    return m_config->getInteger(MAIN_GRP, UPDATE_INTERVAL_KEY, 1);
}

void
StarDraw::setIntervalMinutes(int intervalMinutes)
{
    return m_config->setInteger(MAIN_GRP, UPDATE_INTERVAL_KEY, intervalMinutes);
}

Pango::FontDescription
StarDraw::getStarFont()
{
    return m_config->getFont(MAIN_GRP, STAR_FONT_KEY, DEFAULT_STAR_FONT);
}

void
StarDraw::setStarFont(const Pango::FontDescription& descr)
{
    m_config->setFont(MAIN_GRP, STAR_FONT_KEY, descr);
}

Pango::FontDescription
StarDraw::getCalendarFont()
{
    return m_config->getFont(MAIN_GRP, CALENDAR_FONT_KEY, DEFAULT_CALENDAR_FONT);
}

void
StarDraw::setCalendarFont(Pango::FontDescription& descr)
{
    m_config->setFont(MAIN_GRP, CALENDAR_FONT_KEY, descr);
}

Pango::FontDescription
StarDraw::getInfoFont()
{
    return m_config->getFont(MAIN_GRP, INFO_FONT_KEY, DEFAULT_INFO_FONT);
}

void
StarDraw::setInfoFont(const Pango::FontDescription& descr)
{
    m_config->setFont(MAIN_GRP, INFO_FONT_KEY, descr);
}

Gdk::RGBA
StarDraw::getStartColor()
{
    Gdk::RGBA dfltStart{"rgb(6%,6%,15%)"};
    return m_config->getColor(MAIN_GRP, START_COLOR_KEY, dfltStart);
}

void
StarDraw::setStartColor(const Gdk::RGBA& startColor)
{
    m_config->setColor(MAIN_GRP, START_COLOR_KEY, startColor);
}


Gdk::RGBA
StarDraw::getStopColor()
{
    Gdk::RGBA dfltStop{"rgb(10%,10%,20%)"};
    return m_config->getColor(MAIN_GRP, STOP_COLOR_KEY, dfltStop);
}

void
StarDraw::setStopColor(const Gdk::RGBA& stopColor)
{
    m_config->setColor(MAIN_GRP, STOP_COLOR_KEY, stopColor);
}

Gdk::RGBA
StarDraw::getInfoColor()
{
    Gdk::RGBA dfltInfo{"rgb(50%,50%,50%)"};
    return m_config->getColor(MAIN_GRP, INFO_COLOR_KEY, dfltInfo);
}

void
StarDraw::setInfoColor(const Gdk::RGBA& infoColor)
{
    m_config->setColor(MAIN_GRP, INFO_COLOR_KEY, infoColor);
}

void
StarDraw::getInfoColor(const Cairo::RefPtr<Cairo::Context>& ctx)
{
    auto infoColor = getInfoColor();
    ctx->set_source_rgb(infoColor.get_red(), infoColor.get_green(), infoColor.get_blue());
}

Gdk::RGBA
StarDraw::getCalendarColor()
{
    Gdk::RGBA dfltCalendar{"rgb(50%,50%,50%)"};
    return m_config->getColor(MAIN_GRP, CALENDAR_COLOR_KEY, dfltCalendar);
}

void
StarDraw::setCalendarColor(const Gdk::RGBA& calColor)
{
    m_config->setColor(MAIN_GRP, CALENDAR_COLOR_KEY, calColor);
}

void
StarDraw::getCalendarColor(const Cairo::RefPtr<Cairo::Context>& ctx)
{
    auto infoColor = getCalendarColor();
    ctx->set_source_rgb(infoColor.get_red(), infoColor.get_green(), infoColor.get_blue());

}
