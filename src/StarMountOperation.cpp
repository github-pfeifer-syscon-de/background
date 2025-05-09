/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
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

#include <iostream>

#include "StarMountOperation.hpp"
#include "BackgroundApp.hpp"
#include "StarWin.hpp"

UserDlg::UserDlg(BaseObjectType* cobject
        , const Glib::RefPtr<Gtk::Builder>& builder
        , StarMountOp* starMount
        , const Glib::ustring& message
        , const Glib::ustring &default_user
        , const Glib::ustring &default_domain
        , Gio::AskPasswordFlags flags)
: Gtk::Dialog(cobject)
, m_starMount{starMount}
{
    builder->get_widget("user", m_user);
    m_user->set_text(default_user);
    m_user->set_sensitive(flags & Gio::AskPasswordFlags::ASK_PASSWORD_NEED_USERNAME);
    builder->get_widget("password", m_password);
    m_password->set_sensitive(flags & Gio::AskPasswordFlags::ASK_PASSWORD_NEED_PASSWORD);
    builder->get_widget("domain", m_domain);
    m_domain->set_text(default_domain);
    m_domain->set_sensitive(flags & Gio::AskPasswordFlags::ASK_PASSWORD_NEED_DOMAIN);
}

Glib::ustring
UserDlg::getUser()
{
    return m_user->get_text();
}

Glib::ustring
UserDlg::getPassword()
{
    return m_password->get_text();
}

Glib::ustring
UserDlg::getDomain()
{
    return m_domain->get_text();
}

void
UserDlg::show(StarWin* starWin
           , StarMountOp* starMount
           , const Glib::ustring& message
           , const Glib::ustring &default_user
           , const Glib::ustring &default_domain
           , Gio::AskPasswordFlags flags)
{
    auto builder = Gtk::Builder::create();
    try {
        auto appl = starWin->getBackgroundAppl();
        builder->add_from_resource(appl->get_resource_base_path() + "/user-dlg.ui");
        UserDlg* userDialog;
        builder->get_widget_derived("UserDlg", userDialog, starMount, message, default_user, default_domain, flags);
        userDialog->set_transient_for(*starWin);
        userDialog->set_title(message);
        if (userDialog->run() == Gtk::RESPONSE_OK) {
            starMount->set_username(userDialog->getUser());
            starMount->set_password(userDialog->getPassword());
            starMount->set_domain(userDialog->getDomain());
        }
        userDialog->hide();
        delete userDialog;
    }
    catch (const Glib::Error &ex) {
        std::cerr << "Unable to load user-dialog: " << ex.what() << std::endl;
    }
}

// this is based on a pure guess as i didn't find a practical example
//   so it might need some tweaking, what will practical getting used

StarMountOp::StarMountOp(StarWin* starWin)
: Gio::MountOperation()
, m_starWin{starWin}
{
}

Glib::RefPtr<StarMountOp>
StarMountOp::create(StarWin* starWin)
{
    return Glib::RefPtr<StarMountOp>(new StarMountOp(starWin));
}


void
StarMountOp::on_ask_password(const Glib::ustring& message
        , const Glib::ustring &default_user
        , const Glib::ustring &default_domain
        , Gio::AskPasswordFlags	flags)
{
    UserDlg::show(m_starWin, this, message, default_user, default_domain, flags);
}

