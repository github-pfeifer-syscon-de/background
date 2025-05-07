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
#include "StarDraw.hpp"
#include "StarWin.hpp"
#include "BackgroundApp.hpp"

ParamDlg::ParamDlg(BaseObjectType* cobject
                , const Glib::RefPtr<Gtk::Builder>& builder
                , StarDraw* starDraw)
: Gtk::Dialog(cobject)
, m_starDraw{starDraw}
{
    auto geoPos = m_starDraw->getGeoPosition();
    builder->get_widget("longitude", m_longitude);
    m_longitude->set_value(geoPos.getLonDegrees());
    m_longitude->signal_value_changed().connect(sigc::mem_fun(*this, &ParamDlg::on_time_changed));
    builder->get_widget("latitude", m_latitude);
    m_latitude->set_value(geoPos.getLatDegrees());
    m_latitude->signal_value_changed().connect(sigc::mem_fun(*this, &ParamDlg::on_time_changed));

    builder->get_widget("calendar", m_calendar);
    m_calendar->signal_day_selected().connect(sigc::mem_fun(*this, &ParamDlg::on_time_changed));
    // m_calendar requires set date ?

    Glib::DateTime localNow = Glib::DateTime::create_now_local();
    builder->get_widget("hour", m_spinH);
    m_spinH->set_value(localNow.get_hour());
    m_spinH->signal_value_changed().connect(sigc::mem_fun(*this, &ParamDlg::on_time_changed));
    builder->get_widget("minute", m_spinM);
    m_spinM->set_value(localNow.get_minute());
    m_spinM->signal_value_changed().connect(sigc::mem_fun(*this, &ParamDlg::on_time_changed));

    builder->get_widget("updateInterval", m_updateInterval);
    m_updateInterval->set_value(m_starDraw->getIntervalMinutes());

    builder->get_widget("startColor", m_startColor);
    m_startColor->set_rgba(m_starDraw->getStartColor());

    builder->get_widget("stopColor", m_stopColor);
    m_stopColor->set_rgba(m_starDraw->getStopColor());

    builder->get_widget("starFont", m_starFont);
    m_starFont->set_font_name(m_starDraw->getStarFont().to_string());

    builder->get_widget("calendarColor", m_calendarColor);
    m_calendarColor->set_rgba(m_starDraw->getCalendarModule()->getPrimaryColor());

    builder->get_widget("calendarFont", m_calendarFont);
    m_calendarFont->set_font_name(m_starDraw->getCalendarModule()->getFont().to_string());

    builder->get_widget("infoColor", m_infoColor);
    m_infoColor->set_rgba(m_starDraw->getInfoModule()->getPrimaryColor());

    builder->get_widget("infoFont", m_infoFont);
    m_infoFont->set_font_name(m_starDraw->getInfoModule()->getFont().to_string());

    builder->get_widget("clockColor", m_clockColor);
    m_clockColor->set_rgba(m_starDraw->getClockModule()->getPrimaryColor());

    builder->get_widget("infoPos", m_infoPos);
    fillPos(m_infoPos);
    m_infoPos->set_active_id(m_starDraw->getInfoModule()->getPosition());
    builder->get_widget("calPos", m_calPos);
    fillPos(m_calPos);
    m_calPos->set_active_id(m_starDraw->getCalendarModule()->getPosition());
    builder->get_widget("clockPos", m_clockPos);
    fillPos(m_clockPos);
    m_clockPos->set_active_id(m_starDraw->getClockModule()->getPosition());
    builder->get_widget("clockRadius", m_clockRadius);
    m_clockRadius->set_value(m_starDraw->getClockModule()->getRadius());
	show_all_children();
}

void
ParamDlg::fillPos(Gtk::ComboBoxText* pos)
{
    pos->append("", "None");
    pos->append(POS_TOP, "Top");
    pos->append(POS_MIDDLE, "Middle");
    pos->append(POS_BOTTOM, "Bottom");
}


void
ParamDlg::on_time_changed()
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
    m_starDraw->update(localNow.to_utc(), geoPos);
}

void
ParamDlg::on_response(int response_id)
{
    if (response_id == Gtk::RESPONSE_OK) {
        m_starDraw->setGeoPosition(getGeoPosition());
        m_starDraw->setIntervalMinutes(m_updateInterval->get_value_as_int());
        m_starDraw->setStartColor(m_startColor->get_rgba());
        m_starDraw->setStopColor(m_stopColor->get_rgba());
        Pango::FontDescription starFont{m_starFont->get_font_name()};
        m_starDraw->setStarFont(starFont);
        m_starDraw->getCalendarModule()->setPrimaryColor(m_calendarColor->get_rgba());
        Pango::FontDescription calFont{m_calendarFont->get_font_name()};
        m_starDraw->getCalendarModule()->setFont(calFont);
        m_starDraw->getInfoModule()->setPrimaryColor(m_infoColor->get_rgba());
        Pango::FontDescription infoFont{m_infoFont->get_font_name()};
        m_starDraw->getInfoModule()->setFont(infoFont);
        m_starDraw->getClockModule()->setPrimaryColor(m_clockColor->get_rgba());
        m_starDraw->getInfoModule()->setPosition(m_infoPos->get_active_id());
        m_starDraw->getClockModule()->setPosition(m_clockPos->get_active_id());
        m_starDraw->getCalendarModule()->setPosition(m_calPos->get_active_id());
        m_starDraw->getClockModule()->setRadius(m_clockRadius->get_value());
    }
     Gtk::Dialog::on_response(response_id);
}

GeoPosition
ParamDlg::getGeoPosition() const
{
    GeoPosition geoPos{m_longitude->get_value(), m_latitude->get_value()};
    return geoPos;
}

void
ParamDlg::show(StarDraw* starDraw)
{
    auto builder = Gtk::Builder::create();
    try {
        auto appl = starDraw->getWindow()->getBackgroundAppl();
        builder->add_from_resource(appl->get_resource_base_path() + "/pref-dlg.ui");
        ParamDlg* paramDialog;
        builder->get_widget_derived("PrefDlg", paramDialog, starDraw);
        //auto icon = Gdk::Pixbuf::create_from_resource(appl->get_resource_base_path() + "/background.png");
        //paramDialog->set_logo(icon);
        paramDialog->set_transient_for(*starDraw->getWindow());
        if (paramDialog->run() == Gtk::RESPONSE_OK) {
            starDraw->saveConfig();
            // compute is called from starDraw
        }
        paramDialog->hide();
        delete paramDialog;
    }
    catch (const Glib::Error &ex) {
        std::cerr << "Unable to load pref-dialog: " << ex.what() << std::endl;
    }
}