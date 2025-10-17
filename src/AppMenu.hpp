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

#include <gtkmm.h>
#include <garcon-gtk/garcon-gtk.h>

class StarWin;

class AppMenu
{
public:
    AppMenu();
    explicit AppMenu(const AppMenu& orig) = delete;
    virtual ~AppMenu();

    void on_menu_show(StarWin *starWin);
    void addMenu(Gtk::Menu* popup);

private:
    GarconMenu* m_garcon_menu{nullptr};
    GtkWidget* m_menuWidget{nullptr};
};

