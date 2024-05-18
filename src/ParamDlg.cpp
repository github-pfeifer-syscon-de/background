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

#include "GeoPosition.hpp"
#include "StarDraw.hpp"
#include "ParamDlg.hpp"

ParamDlg::ParamDlg(Gtk::Container* parent, StarDraw* starDraw)
: m_starDraw{starDraw}
{
	set_transient_for(*static_cast<Gtk::Window*>(parent));
	set_title("Parameter");
	auto grid = Gtk::make_managed<Gtk::Grid>();

    auto geoPos = m_starDraw->getGeoPosition();
    m_longitude = Gtk::make_managed<Gtk::SpinButton>();
    m_longitude->set_digits(0);
    m_longitude->set_range(-180.0, 180.0);
    m_longitude->set_increments(1.0, 10.0);
    m_longitude->set_value(geoPos.getLonDegrees());
	auto lblLon = Gtk::make_managed<Gtk::Label>("Longitude ");
	grid->attach(*lblLon, 0, 0, 1, 1);
	grid->attach(*m_longitude, 1, 0, 2, 1);
    m_longitude->signal_value_changed().connect(sigc::mem_fun(*this, &ParamDlg::on_time_changed));
    m_latitude = Gtk::make_managed<Gtk::SpinButton>();
    m_latitude->set_digits(0);
    m_latitude->set_range(-90.0, 90.0);
    m_latitude->set_increments(1.0, 5.0);
    m_latitude->set_value(geoPos.getLatDegrees());
	auto lblLat = Gtk::make_managed<Gtk::Label>("Latitude ");
	grid->attach(*lblLat, 0, 1, 1, 1);
	grid->attach(*m_latitude, 1, 1, 2, 1);
    m_latitude->signal_value_changed().connect(sigc::mem_fun(*this, &ParamDlg::on_time_changed));

	m_calendar = Gtk::make_managed<Gtk::Calendar>();
	auto lblCalendar = Gtk::make_managed<Gtk::Label>("Calendar ");
	grid->attach(*lblCalendar, 0, 2, 1, 1);
	grid->attach(*m_calendar, 1, 2, 2, 1);
    m_calendar->signal_day_selected().connect(sigc::mem_fun(*this, &ParamDlg::on_time_changed));

	m_spinH = Gtk::make_managed<Gtk::SpinButton>();
    m_spinH->set_digits(0);
    m_spinH->set_range(0.0, 23.0);
    m_spinH->set_increments(1.0, 3.0);
	m_spinM = Gtk::make_managed<Gtk::SpinButton>();
    m_spinM->set_digits(0);
    m_spinM->set_range(0.0, 59.0);
    m_spinM->set_increments(1.0, 5.0);
	auto lblTime = Gtk::make_managed<Gtk::Label>("Time ");
	grid->attach(*lblTime, 0, 3, 1, 1);
	grid->attach(*m_spinH, 1, 3, 1, 1);
	grid->attach(*m_spinM, 2, 3, 1, 1);
    Glib::DateTime localNow = Glib::DateTime::create_now_local();
    m_spinH->set_value(localNow.get_hour());
    m_spinM->set_value(localNow.get_minute());
    m_spinH->signal_value_changed().connect(sigc::mem_fun(*this, &ParamDlg::on_time_changed));
    m_spinM->signal_value_changed().connect(sigc::mem_fun(*this, &ParamDlg::on_time_changed));

	get_vbox()->pack_start(*grid);

	auto button_Close = Gtk::make_managed<Gtk::Button>("Ok");
	add_action_widget(*button_Close, Gtk::RESPONSE_OK);

	//m_Depth.grab_default();
	show_all_children();
}

void
ParamDlg::on_time_changed()
{
    Glib::Date calDay;
    m_calendar->get_date(calDay);
    Glib::DateTime localNow = Glib::DateTime::create_local(
            calDay.get_year(), calDay.get_month(), calDay.get_day()
            , static_cast<int>(m_spinH->get_value()), static_cast<int>(m_spinM->get_value()), 0);
    //std::cout << "ValueH " << m_spinH->get_value()
    //          << " M " << m_spinM->get_value()
    //          << " date " << localNow.format_iso8601() << std::endl;
    auto geoPos = getGeoPosition();
    m_starDraw->update(localNow.to_utc(), geoPos);
}

GeoPosition
ParamDlg::getGeoPosition() const
{
    GeoPosition geoPos{m_longitude->get_value(), m_latitude->get_value()};
    return geoPos;
}


