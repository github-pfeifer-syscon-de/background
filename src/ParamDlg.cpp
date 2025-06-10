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

#include "StarDraw.hpp"
#include "ParamDlg.hpp"
#include "StarWin.hpp"
#include "BackgroundApp.hpp"

ParamDlg::ParamDlg(BaseObjectType* cobject
                , const Glib::RefPtr<Gtk::Builder>& builder
                , StarWin* starWin)
: Gtk::Dialog(cobject)
, m_starWin{starWin}
{
    builder->get_widget("updateInterval", m_updateInterval);
    m_updateInterval->set_value(m_starWin->getIntervalMinutes());

    auto starPaint = m_starWin->getStarPaint();
    builder->get_widget("startColor", m_startColor);
    m_startColor->set_rgba(starPaint->getStartColor());
    m_startColor->signal_color_set().connect([this, starPaint] {
        starPaint->setStartColor(m_startColor->get_rgba());
        m_starWin->update();
    });

    builder->get_widget("stopColor", m_stopColor);
    m_stopColor->set_rgba(starPaint->getStopColor());
    m_stopColor->signal_color_set().connect([this, starPaint] {
        starPaint->setStopColor(m_stopColor->get_rgba());
        m_starWin->update();
    });

    builder->get_widget("starFont", m_starFont);
    m_starFont->set_font_name(m_starWin->getStarPaint()->getStarFont().to_string());
    m_starFont->signal_font_set().connect([this, starPaint] {
        Pango::FontDescription starFont{m_starFont->get_font_name()};
        starPaint->setStarFont(starFont);
        m_starWin->update();
    });

    builder->get_widget("showMilkyway", m_showMilkyway);
    m_showMilkyway->set_active(m_starWin->getStarPaint()->isShowMilkyway());
    m_showMilkyway->signal_clicked().connect([this,starPaint] {
        starPaint->setShowMilkyway(m_showMilkyway->get_active());
        m_starWin->update();
    });

    builder->get_widget("messierVMag", m_messierVMag);
    m_messierVMag->set_value(m_starWin->getStarPaint()->getMessierVMagMin());
    m_messierVMag->signal_value_changed().connect([this,starPaint] {
        starPaint->setMessierVMagMin(m_messierVMag->get_value());
        m_starWin->update();
    });

    for (auto& mod : m_starWin->getStarPaint()->getModules()) {
        mod->setupParam(builder, starWin);
    }
	show_all_children();
}


void
ParamDlg::on_response(int response_id)
{
    bool save = false;
    if (response_id == Gtk::RESPONSE_OK) {
        auto starPaint = m_starWin->getStarPaint();
        m_starWin->setIntervalMinutes(m_updateInterval->get_value_as_int());
        starPaint->setStartColor(m_startColor->get_rgba());
        starPaint->setStopColor(m_stopColor->get_rgba());
        Pango::FontDescription starFont{m_starFont->get_font_name()};
        starPaint->setStarFont(starFont);
        starPaint->setShowMilkyway(m_showMilkyway->get_active());
        starPaint->setMessierVMagMin(m_messierVMag->get_value());
        save = true;
    }
    for (auto& mod : m_starWin->getStarPaint()->getModules()) {
        mod->saveParam(save);
    }
}

void
ParamDlg::show(StarWin* starWin)
{
    auto builder = Gtk::Builder::create();
    try {
        starWin->saveConfig(); // for a new state (first startup) the settings are not yet saved as we may want to restore them, save now
        auto appl = starWin->getBackgroundAppl();
        builder->add_from_resource(appl->get_resource_base_path() + "/pref-dlg.ui");
        ParamDlg* paramDialog;
        builder->get_widget_derived("PrefDlg", paramDialog, starWin);
        //auto icon = Gdk::Pixbuf::create_from_resource(appl->get_resource_base_path() + "/background.png");
        //paramDialog->set_logo(icon);
        paramDialog->set_transient_for(*starWin);
        if (paramDialog->run() == Gtk::RESPONSE_OK) {
            starWin->saveConfig();
            // compute is called from starDraw
        }
        else {
            starWin->loadConfig();
        }
        paramDialog->hide();
        delete paramDialog;
    }
    catch (const Glib::Error &ex) {
        std::cerr << "Unable to load pref-dialog: " << ex.what() << std::endl;
    }
}

