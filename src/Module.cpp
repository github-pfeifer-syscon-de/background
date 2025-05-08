/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf <gpl3@pfeifer-syscon.de>
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



#include "StarDraw.hpp"
#include "SysInfo.hpp"
#include "Math.hpp"


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
    int cellWidth = colSpan * m_cellWidth;
    int cellHeight = rowSpan * m_cellHeight;
    // unsure what the exact conversion from pango base line to pixel is 1024 seems to fit here
    ctx->move_to(static_cast<int>(x + (cellWidth - textWidth) * halign)
               , static_cast<int>(y + (cellHeight - baseline / 1024) * valign));
    layout->show_in_cairo_context(ctx);
}

Gdk::RGBA
Module::getPrimaryColor()
{
    Gdk::RGBA dfltColor{"rgb(50%,50%,50%)"};
    return m_config->getColor(getName().c_str(), COLOR_KEY, dfltColor);
}

void
Module::setPrimaryColor(const Gdk::RGBA& primColor)
{
    m_config->setColor(getName().c_str(), COLOR_KEY, primColor);
}

void
Module::getPrimaryColor(const Cairo::RefPtr<Cairo::Context>& ctx)
{
    auto primColor = getPrimaryColor();
    ctx->set_source_rgb(primColor.get_red(), primColor.get_green(), primColor.get_blue());
}


Glib::ustring
Module::getPosition()
{
    return m_config->getString(getName().c_str(), POS_KEY);
}

void
Module::setPosition(const Glib::ustring& position)
{
    m_config->setString(getName().c_str(), POS_KEY, position);
}

Pango::FontDescription
Module::getFont()
{
    return m_config->getFont(getName().c_str(), FONT_KEY, DEFAULT_FONT);
}

void
Module::setFont(Pango::FontDescription& descr)
{
    m_config->setFont(getName().c_str(), FONT_KEY, descr);
}


void
Module::fillPos(Gtk::ComboBoxText* pos)
{
    pos->append("", "None");
    pos->append(POS_TOP, "Top");
    pos->append(POS_MIDDLE, "Middle");
    pos->append(POS_BOTTOM, "Bottom");
}

int
CalendarModule::getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    auto calFont = getFont();
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(calFont);
    pangoLayout->set_text("M"); // use em as reference
    pangoLayout->get_pixel_size(m_width, m_height);
    return 9 * m_height;
}

void
CalendarModule::display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    if (m_height == 0) {
        getHeight(ctx, starDraw);
    }
    // as there seems no way to diffrentiate the locale start with monday
    getPrimaryColor(ctx);
    auto calFont = getFont();
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(calFont);
    starDraw->scale(calFont, 0.6);
    auto smallLayout = Pango::Layout::create(ctx);
    smallLayout->set_font_description(calFont);
    Grid grid{static_cast<int>(m_width * 2.5), m_height};
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
            Gdk::RGBA calColor = getPrimaryColor();
            if (dateTime.get_day_of_month() == dateToday.get_day_of_month()) {
                starDraw->brighten(calColor, 1.3);
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
CalendarModule::setupParam(const Glib::RefPtr<Gtk::Builder>& builder)
{
    builder->get_widget("calendarColor", m_calendarColor);
    m_calendarColor->set_rgba(getPrimaryColor());

    builder->get_widget("calendarFont", m_calendarFont);
    m_calendarFont->set_font_name(getFont().to_string());

    builder->get_widget("calPos", m_calPos);
    fillPos(m_calPos);
    m_calPos->set_active_id(getPosition());
}

void
CalendarModule::saveParam()
{
    setPrimaryColor(m_calendarColor->get_rgba());
    Pango::FontDescription calFont{m_calendarFont->get_font_name()};
    setFont(calFont);
    setPosition(m_calPos->get_active_id());
}

int
InfoModule::getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    auto infoFont = getFont();
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(infoFont);
    pangoLayout->set_text("M");
    int width, height;
    pangoLayout->get_pixel_size(width, height);
    return 6 * height;
}

void
InfoModule::display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    getPrimaryColor(ctx);
    auto infoFont = getFont();
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(infoFont);
    SysInfo sysInfo;
    std::string text;
    text.reserve(512);
    for (auto info : sysInfo.allInfos()) {
        text += info + "\n";
    }
    pangoLayout->set_text(text);
    ctx->move_to(0.0, 0.0);
    pangoLayout->show_in_cairo_context(ctx);
}

void
InfoModule::setupParam(const Glib::RefPtr<Gtk::Builder>& builder)
{
    builder->get_widget("infoColor", m_infoColor);
    m_infoColor->set_rgba(getPrimaryColor());

    builder->get_widget("infoFont", m_infoFont);
    m_infoFont->set_font_name(getFont().to_string());

    builder->get_widget("infoPos", m_infoPos);
    fillPos(m_infoPos);
    m_infoPos->set_active_id(getPosition());
}

void
InfoModule::saveParam()
{
    setPrimaryColor(m_infoColor->get_rgba());
    Pango::FontDescription infoFont{m_infoFont->get_font_name()};
    setFont(infoFont);
    setPosition(m_infoPos->get_active_id());
}

int
ClockModule::getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    if (getDisplay() == DISPLAY_ANALOG) {
        return static_cast<int>(m_radius * 2.0);
    }
    else {
        auto clockFont = getFont();
        auto pangoLayout = Pango::Layout::create(ctx);
        pangoLayout->set_font_description(clockFont);
        pangoLayout->set_text("M"); // use em as reference
        int width, height;
        pangoLayout->get_pixel_size(width, height);
        return static_cast<int>(height * 1.25);
    }
}


// draw line from center outwards
void
ClockModule::drawRadialLine(const Cairo::RefPtr<Cairo::Context>& ctx, int value, int full, double inner, double outer)
{
	double angleRad = 2.0 * M_PI * static_cast<double>(value) / static_cast<double>(full);
	double xv = std::sin(angleRad);
	double yv = -std::cos(angleRad);
    ctx->move_to(inner * xv, inner * yv);
    ctx->line_to(outer * xv, outer * yv);
    ctx->stroke();
}

void
ClockModule::displayAnalog(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    ctx->translate(m_radius, m_radius);
    ctx->begin_new_path();  // as we get a strange stoke otherwise
    ctx->set_line_width(2.0);
    ctx->arc(0.0, 0.0, m_radius, 0, 2.0 * M_PI);
    ctx->stroke();
    Glib::DateTime dateTime = Glib::DateTime::create_now_local();
    drawRadialLine(ctx, dateTime.get_minute(), 60, 0.0, m_radius * 0.8);
    int hourM = (dateTime.get_hour() % 12) * 60 + dateTime.get_minute();
    drawRadialLine(ctx, hourM, 12 * 60, 0.0, m_radius * 0.6);
    ctx->set_line_width(1.0);
	const double inner10 = 0.9 * m_radius;
	const double inner5 = 0.95 * m_radius;
	const double inner2 = 0.98 * m_radius;
	for (int i = 0; i < 60; ++i) {
	    double inner;
	    if (i % 15 == 0) {
            inner = inner10;
	    }
	    else if (i % 5 == 0) {
            inner = inner5;
	    }
	    else {
            inner = inner2;
	    }
	    drawRadialLine(ctx, i, 60, inner, m_radius);
	}
}

void
ClockModule::displayDigital(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    auto fmt = getFormat();
    if (fmt.empty()) {
        fmt = "%X";
    }
    Glib::DateTime dateTime = Glib::DateTime::create_now_local();
    auto clockFont = getFont();
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(clockFont);
    pangoLayout->set_text(dateTime.format(fmt));
    ctx->move_to(0.0, 0.0);
    pangoLayout->show_in_cairo_context(ctx);
}


void
ClockModule::display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    getPrimaryColor(ctx);
    if (getDisplay() == DISPLAY_ANALOG) {
        displayAnalog(ctx, starDraw);
    }
    else {
        displayDigital(ctx, starDraw);
    }

}

void
ClockModule::setupParam(const Glib::RefPtr<Gtk::Builder>& builder)
{
    builder->get_widget("clockColor", m_clockColor);
    m_clockColor->set_rgba(getPrimaryColor());

    builder->get_widget("clockRadius", m_clockRadius);
    m_clockRadius->set_value(getRadius());

    builder->get_widget("clockFormat", m_clockFormat);
    m_clockFormat->set_text(getFormat());

    builder->get_widget("clockFont", m_clockFont);
    m_clockFont->set_font_name(getFont().to_string());

    builder->get_widget("radioAnalog", m_displayAnalog);
    builder->get_widget("radioDigital", m_displayDigital);
    //Gtk::RadioButtonGroup group = m_displayDigital->get_group();
    if (getDisplay() == DISPLAY_ANALOG) {
        m_displayAnalog->set_active(true);
    }
    else {
        m_displayDigital->set_active(true);
    }
    m_displayAnalog->signal_clicked().connect(
            sigc::mem_fun(*this, &ClockModule::update));
    m_displayDigital->signal_clicked().connect(
            sigc::mem_fun(*this, &ClockModule::update));
    update();

    builder->get_widget("clockPos", m_clockPos);
    fillPos(m_clockPos);
    m_clockPos->set_active_id(getPosition());
}

void
ClockModule::update()
{
    if (m_displayAnalog->get_active()) {
        m_clockFormat->set_sensitive(false);
        m_clockFont->set_sensitive(false);
        m_clockRadius->set_sensitive(true);
    }
    else {
        m_clockFormat->set_sensitive(true);
        m_clockFont->set_sensitive(true);
        m_clockRadius->set_sensitive(false);
    }
}

void
ClockModule::saveParam()
{
    setPrimaryColor(m_clockColor->get_rgba());
    setPosition(m_clockPos->get_active_id());
    setRadius(m_clockRadius->get_value());
    Pango::FontDescription clockFont{m_clockFont->get_font_name()};
    setFont(clockFont);
    setDisplay(m_displayAnalog->get_active() ? DISPLAY_ANALOG : DISPLAY_DIGITAL);
    setFormat(m_clockFormat->get_text());
}
