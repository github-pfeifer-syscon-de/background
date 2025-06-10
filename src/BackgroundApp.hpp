/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2020 rpf
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

#include <glibmm.h>

class StarOptionGroup
: public Glib::OptionGroup
{
public:
    StarOptionGroup();

protected:
    bool on_pre_parse(Glib::OptionContext& context, Glib::OptionGroup& group) override;
    bool on_post_parse(Glib::OptionContext& context, Glib::OptionGroup& group) override;
    void on_error(Glib::OptionContext& context, Glib::OptionGroup& group) override;


public:
    bool m_arg_daemon;
    //Glib::OptionGroup::vecustrings m_arg_list;
    Glib::OptionGroup::vecustrings m_remaining_list;
};

class StarWin;
/*
 * get the application up and running
 *   about and help dialog
 */
class BackgroundApp
: public Gtk::Application
{
public:
    BackgroundApp(int arc, char **argv);
    explicit BackgroundApp(const BackgroundApp& nomadApp) = delete;
    virtual ~BackgroundApp() = default;

    void on_activate() override;
    void on_startup() override;
    Glib::RefPtr<Gtk::Builder> get_menu_builder();

    Glib::ustring get_exec_path() const;

    void on_open(const Gio::Application::type_vec_files& files, const Glib::ustring& hint) override;
    void on_action_about();
    bool isDaemon();
protected:
    StarWin* getOrCreateStarWindow();
    StarWin* createStarWindow();

private:
    StarWin* m_starAppWindow{nullptr};
    Glib::ustring m_exec;
    //Glib::RefPtr<Gtk::Builder> m_builder;
    StarOptionGroup m_group;
    void on_action_quit();
    void on_action_help();
};
