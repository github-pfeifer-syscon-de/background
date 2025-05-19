/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
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

#pragma once

#include <gtkmm.h>

#include "config.h"

#ifdef USE_PYTHON
#include "PyWrapper.hpp"
#else
class PyWrapper;
class PyClass;
#endif

class Grid
{
public:
    Grid(int cellWidth, int cellHeight)
    : m_cellWidth{cellWidth}
    , m_cellHeight{cellHeight}
    {
    }
    void put(Glib::RefPtr<Pango::Layout>& layout
           , const Cairo::RefPtr<Cairo::Context>& ctx
           , int col, int row
           , double halign = 0.0, int colSpan = 1
           , double valign = 1.0, int rowSpan = 1);
protected:
private:
    int m_cellWidth;
    int m_cellHeight;
};

class StarDraw;

class Module
{
public:
    Module(std::string name, const std::shared_ptr<KeyConfig>& config, std::shared_ptr<PyWrapper> pyWrapper)
    : m_name{name}
    , m_config{config}
    , m_pyWrapper{pyWrapper}
    {
    }
    explicit Module(const Module& orig) = delete;
    virtual ~Module() = default;

    virtual int getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) = 0;
    virtual void display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) = 0;
    virtual void setupParam(const Glib::RefPtr<Gtk::Builder>& builder) = 0;
    virtual void saveParam() = 0;
    std::string getName()
    {
        return m_name;
    }
    Gdk::RGBA getPrimaryColor();
    void setPrimaryColor(const Gdk::RGBA& primColor);
    void getPrimaryColor(const Cairo::RefPtr<Cairo::Context>& ctx);
    Glib::ustring getPosition();
    void setPosition(const Glib::ustring& position);
    Pango::FontDescription getFont();
    void setFont(Pango::FontDescription& descr);

    static constexpr auto COLOR_KEY{"Color"};
    static constexpr auto POS_KEY{"Pos"};
    static constexpr auto FONT_KEY{"Font"};
    static constexpr auto DEFAULT_FONT{"Sans 12"};
    static constexpr auto POS_TOP{"top"};
    static constexpr auto POS_MIDDLE{"mid"};
    static constexpr auto POS_BOTTOM{"bot"};

protected:
    void fillPos(Gtk::ComboBoxText* pos);
    std::shared_ptr<PyClass> checkPyClass(StarDraw* starDraw, const char* pyfile, const char* className);

    const std::string m_name;
    Glib::ustring m_position;
    Gdk::RGBA m_primaryColor;
    std::shared_ptr<KeyConfig> m_config;
    std::shared_ptr<PyWrapper> m_pyWrapper;
private:
    std::shared_ptr<PyClass> m_pyClass;
};

using PtrModule = std::shared_ptr<Module>;


class CalendarModule
: public Module
{
public:
    CalendarModule(const std::shared_ptr<KeyConfig>& config, const std::shared_ptr<PyWrapper>& pyWrapper)
    : Module{"calendar", config, pyWrapper}
    {
    }
    explicit CalendarModule(const CalendarModule& orig) = delete;
    virtual ~CalendarModule() = default;

    int getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;
    void display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;
    void setupParam(const Glib::RefPtr<Gtk::Builder>& builder) override;
    void saveParam() override;

private:
    int m_width;
    int m_height{0};
    Gtk::ColorButton* m_calendarColor;
    Gtk::FontButton* m_calendarFont;
    Gtk::ComboBoxText* m_calPos;
};

class InfoModule
: public Module
{
public:
    InfoModule(const std::shared_ptr<KeyConfig>& config, const std::shared_ptr<PyWrapper>& pyWrapper)
    : Module{"info", config, pyWrapper}
    {
    }
    explicit InfoModule(const InfoModule& orig) = delete;
    virtual ~InfoModule() = default;

    int getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;
    void display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;
    void setupParam(const Glib::RefPtr<Gtk::Builder>& builder) override;
    void saveParam() override;
private:
    Gtk::ColorButton* m_infoColor;
    Gtk::FontButton* m_infoFont;
    Gtk::ComboBoxText* m_infoPos;
};


class ClockModule
: public Module
{
public:
    ClockModule(const std::shared_ptr<KeyConfig>& config, const std::shared_ptr<PyWrapper>& pyWrapper)
    : Module{"clock", config, pyWrapper}
    {
    }
    explicit ClockModule(const ClockModule& orig) = delete;
    virtual ~ClockModule() = default;

    int getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;
    void display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;
    void setupParam(const Glib::RefPtr<Gtk::Builder>& builder) override;
    void saveParam() override;
    static constexpr auto RADIUS_KEY{"radius"};
    static constexpr auto FORMAT{"format"};
    static constexpr auto DISPLAY_ANALOG{"analog"};
    static constexpr auto DISPLAY_DIGITAL{"digital"};
    double getRadius();
    void setRadius(double radius);
    Glib::ustring getFormat();
    void setFormat(const Glib::ustring& format);
    bool isDisplayAnalog();
    void setDisplayAnalog(bool displayAnalog);
    bool isDisplayDigital();
    void setDisplayDigital(bool displayDigital);
protected:
    void update();
    void drawRadialLine(const Cairo::RefPtr<Cairo::Context>& ctx, int value, int full, bool emphasis, double outer);
    void drawHand(const Cairo::RefPtr<Cairo::Context>& ctx, int value, int full, double outer, double width);
    void displayAnalog(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw);
    void displayDigital(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw, bool center);
    Glib::RefPtr<Pango::Layout> createLayout(const Cairo::RefPtr<Cairo::Context>& ctx);

    Glib::ustring getEffectiveFormat();

    Gtk::ColorButton* m_clockColor;
    Gtk::Scale* m_clockRadius;
    Gtk::Entry* m_clockFormat;
    Gtk::CheckButton* m_displayAnalog;
    Gtk::CheckButton* m_displayDigital;
    Gtk::FontButton* m_clockFont;
    Gtk::ComboBoxText* m_clockPos;
};