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

#include <StringUtils.hpp>

#ifdef __WIN32__
#include <windows.h>
#endif

#include "ClockModule.hpp"
#include "StarDraw.hpp"
#include "Math.hpp"
#include "FileLoader.hpp"
#include "config.h"


Glib::RefPtr<Pango::Layout>
ClockModule::createLayout(const Cairo::RefPtr<Cairo::Context>& ctx)
{
    auto fmt = getEffectiveFormat();
    Glib::DateTime dateTime = Glib::DateTime::create_now_local();
    auto clockFont = getFont();
    auto pangoLayout = Pango::Layout::create(ctx);
    pangoLayout->set_line_spacing(1.2f);    // move lines out of center
    pangoLayout->set_font_description(clockFont);
    pangoLayout->set_text(dateTime.format(fmt));
    return pangoLayout;
}

int
ClockModule::getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    int analogHeight{},digitalHeight{};
#   ifdef USE_PYTHON
    auto pyClass = checkPyClass(starDraw, pyClassName);
    if (!pyClass) {
        std::cout << "ClockModule::getHeight no Class!" << std::endl;
        return 0;
    }
    if (isDisplayAnalog()) {
        analogHeight = static_cast<int>(pyClass->invokeMethod("getAnalogHeight", ctx, getRadius()));
        if (pyClass->hasFailed()) {
            starDraw->showError(pyClass->getError());
            analogHeight = 0;
        }
    }
    if (isDisplayDigital()) {
        auto fontDesc = getFont();
        auto fontName = fontDesc.to_string();
        digitalHeight = static_cast<int>(pyClass->invokeMethod("getDigitalHeight", ctx, fontName, getFormat()));
        if (pyClass->hasFailed()) {
            starDraw->showError(pyClass->getError());
            digitalHeight = 0;
        }
    }
#   else
    if (isDisplayAnalog()) {
        analogHeight = static_cast<int>(getRadius() * 2.0);
    }
    if (isDisplayDigital()) {
        auto pangoLayout = createLayout(ctx);
        int width;
        pangoLayout->get_pixel_size(width, digitalHeight);
    }
#   endif
    return std::max(analogHeight, digitalHeight);
}

double
ClockModule::getRadius()
{
    return m_config->getDouble(getName().c_str(), RADIUS_KEY, 160.0);
}

void
ClockModule::setRadius(double radius)
{
    m_config->setDouble(getName().c_str(), RADIUS_KEY, radius);
}

Glib::ustring
ClockModule::getFormat()
{
    return m_config->getString(getName().c_str(), FORMAT, "%X");
}

void
ClockModule::setFormat(const Glib::ustring& format)
{
    return m_config->setString(getName().c_str(), FORMAT, format);
}

bool
ClockModule::isDisplayAnalog()
{
    return m_config->getBoolean(getName().c_str(), DISPLAY_ANALOG, true);
}

void
ClockModule::setDisplayAnalog(bool displayAnalog)
{
    return m_config->setBoolean(getName().c_str(), DISPLAY_ANALOG, displayAnalog);
}

bool
ClockModule::isDisplayDigital()
{
    return m_config->getBoolean(getName().c_str(), DISPLAY_DIGITAL, false);
}

void
ClockModule::setDisplayDigital(bool displayDigital)
{
    return m_config->setBoolean(getName().c_str(), DISPLAY_DIGITAL, displayDigital);
}


// draw line from center outwards
void
ClockModule::drawRadialLine(const Cairo::RefPtr<Cairo::Context>& ctx, int value, int full, bool emphasis, double outer)
{
	double angleRad = Math::TWO_PI * static_cast<double>(value) / static_cast<double>(full);
	double xv = std::sin(angleRad);
	double yv = -std::cos(angleRad);
    // tick style
	double inner;
	if (emphasis) {
        ctx->set_line_width(2.0);
        inner = outer * 0.9;
	}
	else {
        ctx->set_line_width(1.0);
        inner = outer * 0.95;
	}
    ctx->move_to(inner * xv, inner * yv);
    ctx->line_to(outer * xv, outer * yv);
    ctx->stroke();
    // dot style
    //ctx->arc(xv * outer * 0.9, yv * outer * 0.9, emphasis ? 3.0 : 1.5, 0.0, Math::TWO_PI);
    //ctx->fill();
}

// draw line from center outwards
void
ClockModule::drawHand(const Cairo::RefPtr<Cairo::Context>& ctx, int value, int full, double outer, double width)
{
	double angleRad = Math::TWO_PI * static_cast<double>(value) / static_cast<double>(full);
	double xv = std::sin(angleRad);
	double yv = -std::cos(angleRad);
    ctx->set_line_width(width);
    ctx->set_line_cap(Cairo::LineCap::LINE_CAP_ROUND);
    ctx->move_to(0.0, 0.0);
    ctx->line_to(outer * xv, outer * yv);
    ctx->stroke();

    // if you like more pointy hands
    //ctx->save();
    //ctx->rotate(angleRad + Math::PI);
    //ctx->move_to(-width, 0.0);
    //ctx->line_to(width, 0.0);
    //ctx->line_to(0.0, outer);
    //ctx->close_path();
    //ctx->fill();
    //ctx->restore();
}

// this was kept for reference the drawing was migrated
//    to python to allow more flexibility
void
ClockModule::displayAnalog(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    ctx->save();
    ctx->translate(getRadius(), getRadius());
    ctx->set_line_cap(Cairo::LineCap::LINE_CAP_BUTT);
    ctx->begin_new_path();  // as we get a strange stoke otherwise
    ctx->set_line_width(1.0);
    ctx->arc(0.0, 0.0, getRadius(), 0.0, Math::TWO_PI);
    ctx->stroke();
	//const double inner5 = m_radius * MINUTE_TICK_FACTOR;
    //ctx->arc(0.0, 0.0, inner5, 0.0, Math::TWO_PI);
    //ctx->stroke();

    Glib::DateTime dateTime = Glib::DateTime::create_now_local();
    int hourM = (dateTime.get_hour() % 12) * 60 + dateTime.get_minute();
    drawHand(ctx, hourM, 12 * 60, getRadius() * 0.6, 2.0);
    drawHand(ctx, dateTime.get_minute(), 60, getRadius() * 0.8, 1.0);
	for (int i = 0; i < 60; ++i) {
        drawRadialLine(ctx, i, 60, (i % 5) == 0, getRadius());
	}
    ctx->restore();
}

Glib::ustring
ClockModule::getEffectiveFormat()
{
    auto fmt = getFormat();
    if (fmt.empty()) {
        fmt = "%X";
    }
    fmt = StringUtils::replaceAll(fmt, "\\n", "\n");
    return fmt;
}

void
ClockModule::displayDigital(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw, bool center)
{
    auto pangoLayout = createLayout(ctx);
    if (!center) {
        ctx->move_to(0.0, 0.0);
    }
    else {
        int width, height;
        pangoLayout->get_pixel_size(width, height);
        ctx->move_to(getRadius() - width / 2.0, getRadius() - height / 2.0);
    }
    pangoLayout->show_in_cairo_context(ctx);
}

Glib::ustring
ClockModule::getPyScriptName()
{
    return "clock.py";
}

void
ClockModule::display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw)
{
    getPrimaryColor(ctx);
#   ifdef USE_PYTHON
    auto pyClass = checkPyClass(starDraw, pyClassName);
    if (!pyClass) {
        std::cout << "ClockModule::display no Class!" << std::endl;
        return;
    }
#   endif
    if (isDisplayAnalog()) {
        getPrimaryColor(ctx);   // set the here so we don't have to pass this
        ctx->begin_new_path();  // as we get a strange stoke otherwise
#       ifdef USE_PYTHON
        pyClass->invokeMethod("drawAnalog", ctx, getRadius());
        if (pyClass->hasFailed()) {
            starDraw->showError(pyClass->getError());
        }
#       else
        displayAnalog(ctx, starDraw);
#       endif
    }
    if (isDisplayDigital()) {
        auto color = getPrimaryColor();
        ctx->set_source_rgba(color.get_red(), color.get_green(), color.get_blue(), isDisplayAnalog() ? 0.6 : 1.0);
#       ifdef USE_PYTHON
        auto fmt = getEffectiveFormat();
        auto clockFont = getFont();
        auto fontName= clockFont.to_string();
        pyClass->invokeMethod("drawDigital", ctx, fontName, fmt, isDisplayAnalog() ? getRadius() : 0.0);
        if (pyClass->hasFailed()) {
            starDraw->showError(pyClass->getError());
        }
#       else
        displayDigital(ctx, starDraw, isDisplayAnalog());
#       endif
    }
}

void
ClockModule::setupParam(const Glib::RefPtr<Gtk::Builder>& builder, StarDraw* starDraw)
{
    m_fileLoader = starDraw->getFileLoader();
    builder->get_widget("clockColor", m_clockColor);
    m_clockColor->set_rgba(getPrimaryColor());

    builder->get_widget("clockRadius", m_clockRadius);
    m_clockRadius->set_value(getRadius());

    builder->get_widget("clockFormat", m_clockFormat);
    m_clockFormat->set_text(getFormat());

    builder->get_widget("clockFont", m_clockFont);
    m_clockFont->set_font_name(getFont().to_string());

    builder->get_widget("checkAnalog", m_displayAnalog);
    builder->get_widget("checkDigital", m_displayDigital);
    m_displayAnalog->set_active(isDisplayAnalog());
    m_displayDigital->set_active(isDisplayDigital());
    m_displayAnalog->signal_clicked().connect(
            sigc::mem_fun(*this, &ClockModule::update));
    m_displayDigital->signal_clicked().connect(
            sigc::mem_fun(*this, &ClockModule::update));
    update();

    builder->get_widget("clockPos", m_clockPos);
    fillPos(m_clockPos);
    m_clockPos->set_active_id(getPosition());

    Gtk::Button* editClock;
    builder->get_widget("editClock", editClock);
    Gtk::Label* clockLabel;
    builder->get_widget("clockLabel", clockLabel);

    builder->get_widget("installFont", m_installFont);
    m_installFont->signal_clicked().connect(
        sigc::bind(
            sigc::mem_fun(*this, &ClockModule::installFont), starDraw));

#   ifdef USE_PYTHON
    editClock->signal_clicked().connect(
        sigc::bind(
            sigc::mem_fun(*this, &ClockModule::edit), starDraw));
    clockLabel->set_text(getEditInfo());
#   else
    editClock->set_sensitive(false);
    clockLabel->set_sensitive(false);
#   endif
}

void
ClockModule::update()
{
    m_clockRadius->set_sensitive(m_displayAnalog->get_active());
    m_clockFormat->set_sensitive(m_displayDigital->get_active());
    m_clockFont->set_sensitive(m_displayDigital->get_active());
}

void
ClockModule::saveParam(bool save)
{
    if (save) {
        setPrimaryColor(m_clockColor->get_rgba());
        setPosition(m_clockPos->get_active_id());
        setRadius(m_clockRadius->get_value());
        Pango::FontDescription clockFont{m_clockFont->get_font_name()};
        setFont(clockFont);
        setDisplayAnalog(m_displayAnalog->get_active());
        setDisplayDigital(m_displayDigital->get_active());
        setFormat(m_clockFormat->get_text());
    }
    stopMonitor();
}

void
ClockModule::installFont(StarDraw* starDraw)
{
    std::shared_ptr<FileLoader> fileLoader = starDraw->getFileLoader();
    const char* FONT_FILE{"DSEG14Modern-Regular.ttf"};
    auto file = fileLoader->findFile(FONT_FILE);
    Glib::ustring msg;
    if (file->query_exists()) {
#       ifdef __WIN32__
        auto path = file->get_path();
        // the installed font doesn't appear in local\AppData\Microsoft\Windows\Fonts
        //    nor in font list ... but it works...
        //int ret = AddFontResourceExA(path.c_str(), 0, 0); succeeds, but doesn't show up either
        if (ret == 0) {
            msg =  Glib::ustring::sprintf("Font was not added %s", path);
        }
#       else
        auto fontsPath = Glib::canonicalize_filename("fonts", Glib::get_user_data_dir());
        auto fontsDir = Gio::File::create_for_path(fontsPath);
        if (!fontsDir->query_exists()) {
            fontsDir->make_directory_with_parents();
        }
        auto fontFile = fontsDir->get_child(FONT_FILE);
        if (file->copy(fontFile, Gio::FileCopyFlags::FILE_COPY_OVERWRITE)) {
#           ifdef __linux
            std::vector<std::string> args;
            args.push_back("/usr/bin/fc-cache");
            GPid pid;
            msg = fileLoader->run(args, &pid);
#           endif
        }
        else {
            msg = Glib::ustring::sprintf("Error copying %s", file->get_parse_name());
        }
#       endif
    }
    else {
        msg = Glib::ustring::sprintf("The source %s was not found.", FONT_FILE);
    }
    if (!msg.empty()) {
        starDraw->showError(msg);
    }
}