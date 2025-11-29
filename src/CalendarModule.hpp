/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf 
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

    int getHeight(const Cairo::RefPtr<Cairo::Context>& ctx, StarWin* starWin) override;
    void display(const Cairo::RefPtr<Cairo::Context>& ctx, StarWin* starWin) override;
    void setupParam(const Glib::RefPtr<Gtk::Builder>& builder, StarWin* starWin) override;

    Glib::ustring getPyScriptName() override;
    static constexpr auto pyClassName{"Cal"};
private:
    int m_width;
    int m_height{0};
};


