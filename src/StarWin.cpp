/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2023 RPf <gpl3@pfeifer-syscon.de>
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

#include <cmath>
#include <iostream>

#include "StarWin.hpp"
#include "StarDraw.hpp"
#include "BackgroundApp.hpp"
#include "StarMountOperation.hpp"
#include "FileLoader.hpp"

StarWin::StarWin(BaseObjectType* cobject
        , const Glib::RefPtr<Gtk::Builder>& builder
        , BackgroundApp* backAppl)
: Gtk::ApplicationWindow(cobject)
, m_backAppl{backAppl}
, m_volumeMonitor{Gio::VolumeMonitor::get()}
, m_cancelable{Gio::Cancellable::create()}
{
    set_title("Stars");
    auto pix = Gdk::Pixbuf::create_from_resource(m_backAppl->get_resource_base_path() + "/background.png");
    set_icon(pix);

    builder->get_widget_derived("drawingArea", m_drawingArea, this);
    set_decorated(false);
    maximize();
    updateTimer();
    signal_hide().connect([this] {
        if (m_timer.connected()) {
            m_timer.disconnect();
        }
    });
}

BackgroundApp*
StarWin::getBackgroundAppl()
{
    return m_backAppl;
}

void
StarWin::updateTimer()
{
    Glib::DateTime dateTime = Glib::DateTime::create_now_local();
    // aim for next minute change,
    //  with some extra to prevent a underrun
    //  (overall there seems rather be a chance to overshoot especially with load)
    uint32_t intervalMinutes = 1;
    if (m_drawingArea) {
        intervalMinutes = m_drawingArea->getIntervalMinutes();
    }
    auto delay = (intervalMinutes * 60000u + 50u) - static_cast<unsigned int>(dateTime.get_seconds() * 1000.0);
    m_timer = Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &StarWin::timeoutHandler),
            delay);
}

bool
StarWin::timeoutHandler()
{
    if (m_drawingArea) {
        m_drawingArea->compute();
    }
    updateTimer();
    return false;
}


void
StarWin::on_mount(Glib::RefPtr<Gio::AsyncResult>& result)
{
    try {
        if (m_activeVolume->mount_finish(result)) {
#           ifdef __unix        // assume that works for unixoid, otherwise use __linux
            auto mount = m_activeVolume->get_mount();
            if (mount) {
                std::string arg0;
                auto vapath_list = Gio::File::create_for_path("/usr/bin/va_list");
                if (vapath_list->query_exists()) {
                    arg0 = vapath_list->get_path();
                }
                else {
                    arg0 = "/usr/bin/xdg-open";      // go with desktop default
                }
                std::vector<std::string> args;
                args.push_back(arg0);
                args.push_back(mount->get_root()->get_uri());
                auto msg = m_drawingArea->getFileLoader()->run(args, &m_pid);
                if (!msg.empty()) {
                    showMessage(
                          Glib::ustring::sprintf("Open %s failed with %s", arg0, msg)
                        , Gtk::MessageType::MESSAGE_ERROR);
                }
            }
#           endif
            //showMessage(Glib::ustring::sprintf("Mount succeeded %s at %s", m_activeVolume->get_name(), mountPoint));
        }
        else {
            showMessage(Glib::ustring::sprintf("Mount %s failed", m_activeVolume->get_name()), Gtk::MessageType::MESSAGE_ERROR);
        }
    }
    catch (const Glib::Error& err) {
        showMessage(Glib::ustring::sprintf("Mount %s failed with %s", m_activeVolume->get_name(), err.what() ), Gtk::MessageType::MESSAGE_ERROR);
    }
    // reset as we are done now
    m_activeVolume.reset();
}

void
StarWin::on_eject(Glib::RefPtr<Gio::AsyncResult>& result)
{
    try {
        if (m_activeVolume->eject_finish(result)) {
            //showMessage(Glib::ustring::sprintf("Eject succeeded %s", m_activeVolume->get_name()));
            // no news, good news
        }
        else {
            showMessage(Glib::ustring::sprintf("Eject %s failed", m_activeVolume->get_name()), Gtk::MessageType::MESSAGE_ERROR);
        }
    }
    catch (const Glib::Error& err) {
        showMessage(Glib::ustring::sprintf("Eject %s failed with %s", m_activeVolume->get_name(), err.what() ), Gtk::MessageType::MESSAGE_ERROR);
    }
    // reset as we are done now
    m_activeVolume.reset();
}


void
StarWin::mount(Glib::RefPtr<Gio::Volume>& volume)
{
    if (m_activeVolume) {
        showMessage(Glib::ustring::sprintf("There is already a active mount %s", m_activeVolume->get_name()), Gtk::MessageType::MESSAGE_WARNING);
        return;
    }
    m_activeVolume = volume;
    if (!m_starMountOp) {
        m_starMountOp = StarMountOp::create(this);
    }
    m_activeVolume->mount(m_starMountOp, sigc::mem_fun(*this, &StarWin::on_mount), m_cancelable);
}

void
StarWin::eject(Glib::RefPtr<Gio::Volume>& volume)
{
    if (m_activeVolume) {
        showMessage(Glib::ustring::sprintf("There is already a active mount %s", m_activeVolume->get_name()), Gtk::MessageType::MESSAGE_WARNING);
        return;
    }
    m_activeVolume = volume;
    if (!m_starMountOp) {
        m_starMountOp = StarMountOp::create(this);
    }
    m_activeVolume->eject(m_starMountOp, sigc::mem_fun(*this, &StarWin::on_eject), m_cancelable);
}

void
StarWin::cancel()
{
    if (m_activeVolume) {   // ask if there is still a active operation
        m_cancelable->cancel();
    }
}


void
StarWin::addMenuItems(Gtk::Menu* pMenuPopup)
{
    // this includes harddisk, but also stick with manufacturer
    //auto drives = m_volumeMonitor->get_connected_drives();
    //for (auto drive : drives) {
    //    std::cout << "Drive " << drive->get_name() << std::endl;
    //}
    if (!m_activeVolume) {
        auto volumes = m_volumeMonitor->get_volumes();
        for (auto volume : volumes) {
            if (volume->can_mount()) {
                auto mount = Gtk::make_managed<Gtk::MenuItem>(Glib::ustring::sprintf("Mount %s", volume->get_name()), true);
                mount->signal_activate().connect(
                    sigc::bind(
                        sigc::mem_fun(*this, &StarWin::mount)
                        , volume));
                pMenuPopup->append(*mount);
            }
            else if(volume->can_eject()) {
                auto eject = Gtk::make_managed<Gtk::MenuItem>(Glib::ustring::sprintf("Eject %s", volume->get_name()), true);
                eject->signal_activate().connect(
                    sigc::bind(
                        sigc::mem_fun(*this, &StarWin::eject)
                        , volume));
                pMenuPopup->append(*eject);
            }
        }
    }
    else {
        auto cancel = Gtk::make_managed<Gtk::MenuItem>(Glib::ustring::sprintf("Cancel %s", m_activeVolume->get_name()), true);
        cancel->signal_activate().connect(
                sigc::mem_fun(*this, &StarWin::cancel));
        pMenuPopup->append(*cancel);
    }

    //auto mounts = m_volumeMonitor->get_mounts();
    //for (auto mount : mounts) {
    //    std::cout << "Mount " << mount->get_name() << std::endl;
    //}
}

void
StarWin::showMessage(const Glib::ustring& msg, Gtk::MessageType msgType)
{
    Gtk::MessageDialog messagedialog(*this, msg, false, msgType);
    messagedialog.run();
    messagedialog.hide();
}

