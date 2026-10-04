/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2018 rpf
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
#include <WeatherConfigGrid.hpp>

#include "BackgroundApp.hpp"
#include "ConfigGridFlights.hpp"
#include "ConfigStarGrid.hpp"
#include "GeoPaint.hpp"
#include "ParamDlg.hpp"
#include "StarDraw.hpp"
#include "StarWin.hpp"
#include "ModulePaint.hpp"

ParamDlg::ParamDlg(BaseObjectType* cobject
                , const Glib::RefPtr<Gtk::Builder>& builder
                , StarWin* starWin)
: Gtk::Dialog(cobject)
, m_starWin{starWin}
{
    builder->get_widget("updateInterval", m_updateInterval);
    m_updateInterval->set_value(m_starWin->getIntervalMinutes());
    builder->get_widget("display", m_display);
    m_display->set_sensitive(m_starWin->getBackgroundAppl()->isDaemon());
    if (m_starWin->getBackgroundAppl()->isDaemon()) {
        auto screen = Gdk::Screen::get_default();
        auto monitorCnt = screen->get_n_monitors();
        for (int i = 0; i < monitorCnt; ++i) {
            auto n = Glib::ustring::sprintf("%d", i);
            Gdk::Rectangle rect;
            screen->get_monitor_geometry(i ,rect);
            auto dispName = screen->get_monitor_plug_name(i);
            auto d = Glib::ustring::sprintf("%d %s %d*%d",i, dispName, rect.get_width(), rect.get_height());
            m_display->append(n, d);
            if (i == m_starWin->getDaemonDisplay()) {
                m_display->set_active_id(n);
            }
        }
    }
    for (auto& mod : m_starWin->getModulePaint()->getModules()) {
        mod->setupParam(builder, starWin);
    }
    builder->get_widget_derived("configStarGrid", m_configStarGrid, starWin);
    builder->get_widget_derived("configWeatherGrid", m_configWeatherGrid, dynamic_cast<BaseConfigListener*>(starWin->getGeoPaint().get()));
    builder->get_widget_derived("configGeoJsonGrid", m_configGeoJson, dynamic_cast<BaseConfigListener*>(starWin->getGeoPaint().get()));
    builder->get_widget_derived("configFlightsGrid", m_flightGrid, starWin->getGeoPaint());

    show_all_children();
}


void
ParamDlg::on_response(int response_id)
{
    bool save = false;
    if (response_id == Gtk::RESPONSE_OK) {
        m_flightGrid->save();
        m_configStarGrid->save();
        m_starWin->setIntervalMinutes(m_updateInterval->get_value_as_int());
        if (m_starWin->getBackgroundAppl()->isDaemon()) {
            auto nMonitor = m_display->get_active_id();
            try {
                int i = std::stoi(nMonitor);
                m_starWin->setDaemonDisplay(i);
            }
            catch (const std::exception& exc) {
                std::cout << "Error parsing select display " << nMonitor << std::endl;
            }
        }
        save = true;
        for (auto& mod : m_starWin->getModulePaint()->getModules()) {
            mod->saveParam(save);
        }
    }
}


