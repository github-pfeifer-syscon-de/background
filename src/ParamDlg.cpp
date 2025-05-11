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

    builder->get_widget("showMilkyway", m_showMilkyway);
    m_showMilkyway->set_active(m_starDraw->isShowMilkyway());

    builder->get_widget("messierVMag", m_messierVMag);
    m_messierVMag->set_value(m_starDraw->getMessierVMagMin());

    m_starDraw->getInfoModule()->setupParam(builder);
    m_starDraw->getCalendarModule()->setupParam(builder);
    m_starDraw->getClockModule()->setupParam(builder);

	show_all_children();
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
        m_starDraw->setShowMilkyway(m_showMilkyway->get_active());
        m_starDraw->setMessierVMagMin(m_messierVMag->get_value());

        m_starDraw->getInfoModule()->saveParam();
        m_starDraw->getCalendarModule()->saveParam();
        m_starDraw->getClockModule()->saveParam();
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