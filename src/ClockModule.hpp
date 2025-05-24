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

#include "Module.hpp"

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
    void setupParam(const Glib::RefPtr<Gtk::Builder>& builder, StarDraw* starDraw) override;
    void saveParam(bool save) override;
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
    Glib::ustring getPyScriptName() override;
    static constexpr auto pyClassName{"Clock"};
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