/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf <gpl3@pfeifer-syscon.de>
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

class StarMountOp;
class StarWin;

class UserDlg
: public Gtk::Dialog
{
public:
    UserDlg( BaseObjectType* cobject
           , const Glib::RefPtr<Gtk::Builder>& builder
           , StarMountOp* starMount
           , const Glib::ustring& message
           , const Glib::ustring &default_user
           , const Glib::ustring &default_domain
           , Gio::AskPasswordFlags flags);
    explicit UserDlg(const UserDlg& orig) = delete;
    virtual ~UserDlg() = default;
    static void show(StarWin* starWin
           , StarMountOp* starMount
           , const Glib::ustring& message
           , const Glib::ustring &default_user
           , const Glib::ustring &default_domain
           , Gio::AskPasswordFlags flags);
    Glib::ustring getUser();
    Glib::ustring getPassword();
    Glib::ustring getDomain();
private:
    Gtk::Entry* m_user;
    Gtk::Entry* m_password;
    Gtk::Entry* m_domain;
    StarMountOp* m_starMount;
    Gio::AskPasswordFlags m_flags;
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

