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


#include "CalendarModule.hpp"
#include "StarDraw.hpp"
#include "Math.hpp"
#include "FileLoader.hpp"
#include "config.h"

void
Grid::put(Glib::RefPtr<Pango::Layout>& layout
           , const Cairo::RefPtr<Cairo::Context>& ctx
           , int col, int row
           , double halign, int colSpan
           , double valign, int rowSpan)
{
    // the pango layout alignment did not work as expected
    // to make text look nicely put it on exact pixel location
    int x = col * m_cellWidth;
    int y = row * m_cellHeight;
    int textWidth, textHeight;
    layout->get_pixel_size(textWidth, textHeight);
    int baseline = layout->get_baseline();
    int cellWidth = colSpan * m_cellWidth;
    int cellHeight = rowSpan * m_cellHeight;
    // unsure what the exact conversion from pango base line to pixel is, 1024 seems to fit here
    ctx->move_to(static_cast<int>(x + (cellWidth - textWidth) * halign)
               , static_cast<int>(y + (cellHeight - baseline / 1024) * valign));
    layout->show_in_cairo_context(ctx);
}


int
CalendarModule::getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    auto calFont = getFont();
#   ifdef USE_PYTHON
    auto pyClass = checkPyClass(starDraw, pyClassName);
    if (!pyClass) {
        std::cout << "CalendarModule::getHeight no Class!" << std::endl;
        return 0;
    }
    auto fontName = calFont.to_string();
    int height = static_cast<int>(pyClass->invokeMethod("getHeight", ctx, fontName));
    if (pyClass->hasFailed()) {
        starDraw->showError(pyClass->getError());
        height = 0;
    }
    return height;
#   else
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(calFont);
    pangoLayout->set_text("M"); // use em as reference
    pangoLayout->get_pixel_size(m_width, m_height);
    return 9 * m_height;
#   endif
}

Glib::ustring
CalendarModule::getPyScriptName()
{
    return "cal.py";
}

void
CalendarModule::display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    if (m_height == 0) {
        getHeight(ctx, starDraw);
    }

    getPrimaryColor(ctx);
    auto calFont = getFont();
#   ifdef USE_PYTHON
    auto pyClass = checkPyClass(starDraw, pyClassName);
    if (pyClass) {
        auto font = calFont.to_string();
        pyClass->invokeMethod("draw", ctx, font);
        if (pyClass->hasFailed()) {
            starDraw->showError(pyClass->getError());
        }
    }
    else {
        std::cout << "CalendarModule::display no Class!" << std::endl;
    }
#   else
    // as there seems no way to diffrentiate the locale start with monday (but it's the iso way)
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_font_description(calFont);
    starDraw->scale(calFont, 0.6);
    auto smallLayout = Pango::Layout::create(ctx);
    smallLayout->set_font_description(calFont);
    auto boldLayout = Pango::Layout::create(ctx);
    auto boldFont = getFont();
    boldFont.set_weight(Pango::WEIGHT_BOLD);
    boldLayout->set_font_description(boldFont);
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
            auto dayLayout = pangoLayout;
            if (dateTime.get_day_of_month() == dateToday.get_day_of_month()) {
                dayLayout = boldLayout;
            }
            dayLayout->set_text(dateTime.format("%e"));
            grid.put(dayLayout, ctx, w, row, 1.0);
            dateTime = dateTime.add_days(1);
            if (dateTime.get_month() != dateToday.get_month()) {
                return;      // if month ended stop
            }
        }
    }
#   endif
}

void
CalendarModule::setupParam(const Glib::RefPtr<Gtk::Builder>& builder, StarDraw* starDraw)
{
    Module::setupParam(builder, starDraw, "calendarColor", "calendarFont", "calPos", "editCal", "calLabel");
}
