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
                , StarDraw* starDraw)
: Gtk::Dialog(cobject)
, m_starDraw{starDraw}
{
    builder->get_widget("updateInterval", m_updateInterval);
    m_updateInterval->set_value(m_starDraw->getIntervalMinutes());

    builder->get_widget("startColor", m_startColor);
    m_startColor->set_rgba(m_starDraw->getStartColor());
    m_startColor->signal_color_set().connect([this] {
        m_starDraw->setStartColor(m_startColor->get_rgba());
        m_starDraw->compute();
    });

    builder->get_widget("stopColor", m_stopColor);
    m_stopColor->set_rgba(m_starDraw->getStopColor());
    m_stopColor->signal_color_set().connect([this] {
        m_starDraw->setStopColor(m_stopColor->get_rgba());
        m_starDraw->compute();
    });

    builder->get_widget("starFont", m_starFont);
    m_starFont->set_font_name(m_starDraw->getStarFont().to_string());
    m_starFont->signal_font_set().connect([this] {
        Pango::FontDescription starFont{m_starFont->get_font_name()};
        m_starDraw->setStarFont(starFont);
        m_starDraw->compute();
    });

    builder->get_widget("showMilkyway", m_showMilkyway);
    m_showMilkyway->set_active(m_starDraw->isShowMilkyway());
    m_showMilkyway->signal_clicked().connect([this] {
        m_starDraw->setShowMilkyway(m_showMilkyway->get_active());
        m_starDraw->compute();
    });

    builder->get_widget("messierVMag", m_messierVMag);
    m_messierVMag->set_value(m_starDraw->getMessierVMagMin());
    m_messierVMag->signal_value_changed().connect([this] {
        m_starDraw->setMessierVMagMin(m_messierVMag->get_value());
        m_starDraw->compute();
    });

    for (auto& mod : m_starDraw->getModules()) {
        mod->setupParam(builder, starDraw);
    }
	show_all_children();
}


void
ParamDlg::on_response(int response_id)
{
    bool save = false;
    if (response_id == Gtk::RESPONSE_OK) {
        m_starDraw->setIntervalMinutes(m_updateInterval->get_value_as_int());
        m_starDraw->setStartColor(m_startColor->get_rgba());
        m_starDraw->setStopColor(m_stopColor->get_rgba());
        Pango::FontDescription starFont{m_starFont->get_font_name()};
        m_starDraw->setStarFont(starFont);
        m_starDraw->setShowMilkyway(m_showMilkyway->get_active());
        m_starDraw->setMessierVMagMin(m_messierVMag->get_value());
        save = true;
    }
    for (auto& mod : m_starDraw->getModules()) {
        mod->saveParam(save);
    }
}

void
ParamDlg::show(StarDraw* starDraw)
{
    auto builder = Gtk::Builder::create();
    try {
        starDraw->saveConfig(); // for a new state (first startup) the settings are not yet saved as we may want to restore them, save now
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
        else {
            starDraw->loadConfig();
        }
        paramDialog->hide();
        delete paramDialog;
    }
    catch (const Glib::Error &ex) {
        std::cerr << "Unable to load pref-dialog: " << ex.what() << std::endl;
    }
}

