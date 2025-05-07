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

#pragma once

#include <gtkmm.h>

class StarDraw;
class GeoPosition;

class ParamDlg
: public Gtk::Dialog
{
public:
    ParamDlg(BaseObjectType* cobject
            , const Glib::RefPtr<Gtk::Builder>& builder
            , StarDraw* starDraw);
    virtual ~ParamDlg() = default;

    GeoPosition getGeoPosition() const;
    static void show(StarDraw* starDraw);
    static constexpr auto POS_TOP{"top"};
    static constexpr auto POS_MIDDLE{"mid"};
    static constexpr auto POS_BOTTOM{"bot"};

protected:
    void on_time_changed();
    void on_response(int response_id) override;
    void fillPos(Gtk::ComboBoxText* pos);

private:
    Gtk::Calendar* m_calendar;
    Gtk::SpinButton* m_spinH;
    Gtk::SpinButton* m_spinM;
    StarDraw* m_starDraw;
    Gtk::SpinButton* m_latitude;
    Gtk::SpinButton* m_longitude;
    Gtk::SpinButton* m_updateInterval;
    Gtk::ColorButton* m_startColor;
    Gtk::ColorButton* m_stopColor;
    Gtk::FontButton* m_starFont;
    Gtk::ColorButton* m_calendarColor;
    Gtk::FontButton* m_calendarFont;
    Gtk::ColorButton* m_infoColor;
    Gtk::FontButton* m_infoFont;
    Gtk::ColorButton* m_clockColor;
    Gtk::ComboBoxText* m_infoPos;
    Gtk::ComboBoxText* m_calPos;
    Gtk::ComboBoxText* m_clockPos;
    Gtk::Scale* m_clockRadius;
};
