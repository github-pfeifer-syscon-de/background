/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
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

#include <iostream>
#include <glib-2.0/gio/gio.h>

#include "AppMenu.hpp"
#include "StarWin.hpp"

AppMenu::AppMenu()
{
}

AppMenu::~AppMenu()
{
    if (m_menuWidget) {
        gtk_widget_unrealize(m_menuWidget);
        m_menuWidget  = nullptr;
    }
    if (m_garcon_menu) {
        g_object_unref(m_garcon_menu);
        m_garcon_menu = nullptr;
    }
}

void
AppMenu::addMenu(Gtk::Menu* pMenuPopup)
{
    if (!m_garcon_menu) {
        m_garcon_menu = garcon_menu_new_applications();
    }
    GList* list = garcon_menu_get_elements(m_garcon_menu);
    GList* elem;
    for(elem = list; elem; elem = elem->next) {
        auto itemElem = GARCON_MENU_ELEMENT(elem->data);
        if (!garcon_menu_element_get_no_display(itemElem)) {
            auto name = garcon_menu_element_get_name(itemElem);
            std::cout << "Name " << name << std::endl;
        }
        g_object_unref(itemElem);
    }
    g_list_free(list);
    //auto appMenu = Glib::wrap(G_MENU_MODEL(m_garcon_menu), true);
    //pMenuPopup->bind_model(appMenu, true);

    return;
}

void
AppMenu::on_menu_show(StarWin *starWin)
{
    if (!m_garcon_menu) {
        m_garcon_menu = garcon_menu_new_applications();
    }
    //GList* list = garcon_menu_get_elements(m_garcon_menu);
    // garcon_menu_get_elements -> garcon_menu_get_elements: assertion 'layout != NULL' failed
    // garcon_menu_get_items -> empty
    //GList* elem;
    //for(elem = list; elem; elem = elem->next) {
    //    auto item = static_cast<GarconMenuElement*>(elem->data);
    //    auto name = garcon_menu_element_get_name(item);
    //    std::cout << "Name " << name << std::endl;
    //    g_object_unref(item);
    //}
    //g_list_free(list);
    if (!m_menuWidget) {
        m_menuWidget = garcon_gtk_menu_new(m_garcon_menu);
        // since we don't deal with actions needed?
        //gtk_menu_attach_to_widget(GTK_MENU(menuWidget), GTK_WIDGET(starWin->gobj()), nullptr);
    }
    GdkEvent* event = gtk_get_current_event();
    gtk_widget_show_all(m_menuWidget);
    gtk_menu_popup_at_pointer(GTK_MENU(m_menuWidget), event);
    // this provokes a free issue!!!
}