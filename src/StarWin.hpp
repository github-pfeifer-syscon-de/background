/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#pragma once

#include <gtkmm.h>
#include <memory>

class StarDraw;
class BackgroundApp;
class StarMountOp;

class StarWin
: public Gtk::ApplicationWindow
{
public:
    StarWin(BaseObjectType* cobject
        , const Glib::RefPtr<Gtk::Builder>& builder
        , BackgroundApp* backAppl);
    explicit StarWin(const StarWin& orig) = delete;
    virtual ~StarWin() = default;

    BackgroundApp* getBackgroundAppl();
    void showMessage(const Glib::ustring& msg, Gtk::MessageType msgType = Gtk::MessageType::MESSAGE_INFO);
    void mount(Glib::RefPtr<Gio::Volume>& volume);
    void eject(Glib::RefPtr<Gio::Volume>& volume);

    void addMenuItems(Gtk::Menu* menu);
protected:
    bool timeoutHandler();
    void updateTimer();
    void cancel();
    void on_mount(Glib::RefPtr<Gio::AsyncResult>& result);
    void on_eject(Glib::RefPtr<Gio::AsyncResult>& result);
private:
    StarDraw* m_drawingArea{nullptr};
    sigc::connection m_timer;
    BackgroundApp* m_backAppl;

    Glib::RefPtr<Gio::VolumeMonitor> m_volumeMonitor;
    Glib::RefPtr<Gio::Cancellable> m_cancelable;
    Glib::RefPtr<StarMountOp> m_starMountOp;
    Glib::RefPtr<Gio::Volume> m_activeVolume;   // e.g. the volume we are currently working on opening, ejecting...
    GPid m_pid;
};


class StarMountOp
: public Gio::MountOperation
{
public:
    explicit StarMountOp(const StarMountOp& orig) = delete;
    virtual ~StarMountOp() = default;
    void on_ask_password(const Glib::ustring& message
                , const Glib::ustring& default_user
                , const Glib::ustring& default_domain
                , Gio::AskPasswordFlags flags) override;
    static Glib::RefPtr<StarMountOp> create(StarWin* starWin);

protected:
    StarMountOp(StarWin* starWin);
private:
    StarWin* m_starWin;
};

