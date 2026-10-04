/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2026 RPf
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


#include "ConfigStarGrid.hpp"
#include "StarWin.hpp"
#include "StarPaint.hpp"

ConfigStarGrid::ConfigStarGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, StarWin* starWin)
: Gtk::Grid(cobject)
, m_starWin{starWin}
{
   auto starPaint = std::dynamic_pointer_cast<StarPaint>(m_starWin->getStarPaint());
    builder->get_widget("startColor", m_startColor);
    if (starPaint) {
        m_startColor->set_rgba(starPaint->getStartColor());
        m_startColor->signal_color_set().connect([this, starPaint] {
            starPaint->setStartColor(m_startColor->get_rgba());
            m_starWin->update();
        });
    }
    else {
        m_startColor->set_sensitive(false);
    }

    builder->get_widget("stopColor", m_stopColor);
    if (starPaint) {
        m_stopColor->set_rgba(starPaint->getStopColor());
        m_stopColor->signal_color_set().connect([this, starPaint] {
            starPaint->setStopColor(m_stopColor->get_rgba());
            m_starWin->update();
        });
    }
    else {
        m_stopColor->set_sensitive(false);
    }

    builder->get_widget("starFont", m_starFont);
    if (starPaint) {
        m_starFont->set_font_name(starPaint->getStarFont().to_string());
        m_starFont->signal_font_set().connect([this, starPaint] {
            Pango::FontDescription starFont{m_starFont->get_font_name()};
            starPaint->setStarFont(starFont);
            m_starWin->update();
        });
    }
    else {
        m_starFont->set_sensitive(false);
    }

    builder->get_widget("showMilkyway", m_showMilkyway);
    if (starPaint) {
        m_showMilkyway->set_active(starPaint->isShowMilkyway());
        m_showMilkyway->signal_clicked().connect([this,starPaint] {
            starPaint->setShowMilkyway(m_showMilkyway->get_active());
            m_starWin->update();
        });
    }
    else {
        m_showMilkyway->set_sensitive(false);
    }

    builder->get_widget("messierVMag", m_messierVMag);
    if (starPaint) {
        m_messierVMag->set_value(starPaint->getMessierVMagMin());
        m_messierVMag->signal_value_changed().connect([this,starPaint] {
            starPaint->setMessierVMagMin(m_messierVMag->get_value());
            m_starWin->update();
        });
    }
    else {
        m_messierVMag->set_sensitive(false);
    }

}

void
ConfigStarGrid::save()
{
    auto starPaint = std::dynamic_pointer_cast<StarPaint>(m_starWin->getStarPaint());
    if (starPaint) {
        starPaint->setStartColor(m_startColor->get_rgba());
        starPaint->setStopColor(m_stopColor->get_rgba());
        Pango::FontDescription starFont{m_starFont->get_font_name()};
        starPaint->setStarFont(starFont);
        starPaint->setShowMilkyway(m_showMilkyway->get_active());
        starPaint->setMessierVMagMin(m_messierVMag->get_value());
    }
}