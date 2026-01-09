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

#include "GeoPosition.hpp"
#include "StarDraw.hpp"
#include "StarWin.hpp"
#include "BackgroundApp.hpp"
#include "TimeDlg.hpp"


TimeDlg::TimeDlg(BaseObjectType* cobject
                , const Glib::RefPtr<Gtk::Builder>& builder
                , StarWin* starWin)
: Gtk::Dialog(cobject)
, m_starWin{starWin}
{
    auto geoPos = m_starWin->getGeoPosition();
    builder->get_widget("longitude", m_longitude);
    m_longitude->set_value(geoPos.getLonDegrees());
    m_longitude->signal_value_changed().connect(sigc::mem_fun(*this, &TimeDlg::on_time_changed));
    builder->get_widget("latitude", m_latitude);
    m_latitude->set_value(geoPos.getLatDegrees());
    m_latitude->signal_value_changed().connect(sigc::mem_fun(*this, &TimeDlg::on_time_changed));

    builder->get_widget("calendar", m_calendar);
    m_calendar->signal_day_selected().connect(sigc::mem_fun(*this, &TimeDlg::on_time_changed));
    // m_calendar requires set date ?

    Glib::DateTime localNow = Glib::DateTime::create_now_local();
    builder->get_widget("hour", m_spinH);
    m_spinH->set_value(localNow.get_hour());
    m_spinH->signal_value_changed().connect(sigc::mem_fun(*this, &TimeDlg::on_time_changed));
    builder->get_widget("minute", m_spinM);
    m_spinM->set_value(localNow.get_minute());
    m_spinM->signal_value_changed().connect(sigc::mem_fun(*this, &TimeDlg::on_time_changed));

	show_all_children();
}


GeoPosition
TimeDlg::getGeoPosition() const
{
    GeoPosition geoPos{m_longitude->get_value(), m_latitude->get_value()};
    return geoPos;
}

void
TimeDlg::on_time_changed()
{
    Glib::Date calDay;
    m_calendar->get_date(calDay);
    Glib::DateTime localNow = Glib::DateTime::create_local(
            calDay.get_year(), calDay.get_month(), calDay.get_day()
            , m_spinH->get_value_as_int()
            , m_spinM->get_value_as_int(), 0);
    //std::cout << "ValueH " << m_spinH->get_value()
    //          << " M " << m_spinM->get_value()
    //          << " date " << localNow.format_iso8601() << std::endl;
    auto geoPos = getGeoPosition();
    m_starWin->update(localNow.to_utc(), geoPos);
}

void
TimeDlg::on_response(int response_id)
{
    if (response_id == Gtk::RESPONSE_OK) {
        m_starWin->setGeoPosition(getGeoPosition());
    }
    Gtk::Dialog::on_response(response_id);
}

void
TimeDlg::show(StarWin* starWin)
{
    auto builder = Gtk::Builder::create();
    try {
        auto appl = starWin->getBackgroundAppl();
        builder->add_from_resource(appl->get_resource_base_path() + "/time-dlg.ui");
        TimeDlg* timeDialog;
        builder->get_widget_derived("TimeDlg", timeDialog, starWin);
        //auto icon = Gdk::Pixbuf::create_from_resource(appl->get_resource_base_path() + "/background.png");
        //paramDialog->set_logo(icon);
        timeDialog->set_transient_for(*starWin);
        if (timeDialog->run() == Gtk::RESPONSE_OK) {
            starWin->savePosition();
        }
        timeDialog->hide();
        delete timeDialog;
    }
    catch (const Glib::Error &ex) {
        std::cerr << "Unable to load time-dialog: " << ex.what() << std::endl;
    }
}