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
#include "BackConfig.hpp"

ConfigStarGrid::ConfigStarGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, StarWin* starWin)
: Gtk::Grid(cobject)
, m_starWin{starWin}
{
    auto starPaint = m_starWin->getStarPaint();
    auto backConfig = m_starWin->getConfig();
    builder->get_widget("startColor", m_startColor);
    m_startColor->set_rgba(backConfig->getStartColor());
    m_startColor->signal_color_set().connect([this, backConfig] {
        backConfig->setStartColor(m_startColor->get_rgba());
        m_starWin->update();
    });

    builder->get_widget("stopColor", m_stopColor);
    m_stopColor->set_rgba(backConfig->getStopColor());
    m_stopColor->signal_color_set().connect([this, backConfig] {
        backConfig->setStopColor(m_stopColor->get_rgba());
        m_starWin->update();
    });

    builder->get_widget("starFont", m_starFont);
    if (starPaint) {
        m_starFont->set_font_name(backConfig->getStarFont().to_string());
        m_starFont->signal_font_set().connect([this, backConfig] {
            Pango::FontDescription starFont{m_starFont->get_font_name()};
            backConfig->setStarFont(starFont);
            m_starWin->update();
        });
    }
    else {
        m_starFont->set_sensitive(false);
    }

    builder->get_widget("showMilkyway", m_showMilkyway);
    m_showMilkyway->set_active(backConfig->isShowMilkyway());
    m_showMilkyway->signal_clicked().connect([this,backConfig] {
        backConfig->setShowMilkyway(m_showMilkyway->get_active());
        m_starWin->update();
    });

    builder->get_widget("messierVMag", m_messierVMag);
    m_messierVMag->set_value(backConfig->getMessierVMagMin());
    m_messierVMag->signal_value_changed().connect([this,backConfig] {
        backConfig->setMessierVMagMin(m_messierVMag->get_value());
        m_starWin->update();
    });
}

void
ConfigStarGrid::save()
{
    auto starPaint = m_starWin->getStarPaint();
    auto backConfig = m_starWin->getConfig();
    if (starPaint) {
        backConfig->setStartColor(m_startColor->get_rgba());
        backConfig->setStopColor(m_stopColor->get_rgba());
        Pango::FontDescription starFont{m_starFont->get_font_name()};
        backConfig->setStarFont(starFont);
        backConfig->setShowMilkyway(m_showMilkyway->get_active());
        backConfig->setMessierVMagMin(m_messierVMag->get_value());
    }
}