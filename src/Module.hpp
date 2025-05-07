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
    Module(std::string name, const std::shared_ptr<KeyConfig>& config)
    : m_name{name}
    , m_config{config}
    {
    }
    explicit Module(const Module& orig) = delete;
    virtual ~Module() = default;

    virtual int getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) = 0;
    virtual void display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) = 0;
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


protected:
    const std::string m_name;
    Glib::ustring m_position;
    Gdk::RGBA m_primaryColor;
    std::shared_ptr<KeyConfig> m_config;
};

using PtrModule = std::shared_ptr<Module>;


class CalendarModule
: public Module
{
public:
    CalendarModule(const std::shared_ptr<KeyConfig>& config)
    : Module{"calendar", config}
    {
    }
    explicit CalendarModule(const CalendarModule& orig) = delete;
    virtual ~CalendarModule() = default;

    int getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;
    void display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;
private:
    int m_width;
    int m_height{0};

};

class InfoModule
: public Module
{
public:
    InfoModule(const std::shared_ptr<KeyConfig>& config)
    : Module{"info", config}
    {
    }
    explicit InfoModule(const InfoModule& orig) = delete;
    virtual ~InfoModule() = default;

    int getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;
    void display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;

};


class ClockModule
: public Module
{
public:
    ClockModule(const std::shared_ptr<KeyConfig>& config)
    : Module{"clock", config}
    , m_radius{getRadius()}
    {
    }
    explicit ClockModule(const ClockModule& orig) = delete;
    virtual ~ClockModule() = default;

    int getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;
    void display(const Cairo::RefPtr<Cairo::Context>& ctx, StarDraw* starDraw) override;
    static constexpr auto RADIUS{"radius"};
    double getRadius()
    {
        return m_config->getDouble(getName().c_str(), RADIUS, 160.0);
    }
    void setRadius(double radius)
    {
        m_radius = radius;
        m_config->setDouble(getName().c_str(), RADIUS, m_radius);
    }
protected:
    void drawRadialLine(const Cairo::RefPtr<Cairo::Context>& ctx, int value, int full, double inner, double outer);
    double m_radius;
};