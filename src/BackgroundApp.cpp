/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2023 rpf
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
#include <iomanip>
#include <iostream>
#include <exception>
#include <KeyConfig.hpp>

#include "config.h"
#include "BackgroundApp.hpp"
#include "StarWin.hpp"
#include "StarPaint.hpp"



StarOptionGroup::StarOptionGroup()
: Glib::OptionGroup(
    "Star desk options", "control the startup for star desk", "use --daemon or -d to startup in daemon mode, otherwise interactive mode is used")
, m_arg_daemon{false}
{

  Glib::OptionEntry entry4;
  entry4.set_long_name("daemon");
  entry4.set_short_name('d');
  entry4.set_description("Activate daemon mode");
  //entry4.set_flags(Glib::OptionEntry::Flags::FLAG_OPTIONAL_ARG);
  add_entry(entry4, m_arg_daemon);

  Glib::OptionEntry entry_remaining;
  entry_remaining.set_long_name(G_OPTION_REMAINING);

  add_entry(entry_remaining, m_remaining_list);
}

void
StarOptionGroup::on_error(Glib::OptionContext& /* context */, Glib::OptionGroup& /* group */)
{
    std::cout << "on_error called" << std::endl;
}


BackgroundApp::BackgroundApp(int argc, char **argv)
: Gtk::Application(argc, argv, "de.pfeifer_syscon.background", Gio::ApplicationFlags::APPLICATION_HANDLES_OPEN)
, m_exec{argv[0]}
{
    Glib::OptionContext context;
    StarOptionGroup group;
    context.set_main_group(group);
    try {
        context.parse(argc, argv);
        m_daemon = group.m_arg_daemon;
    }
    catch (const Glib::Error& ex) {
        std::cout << "Exception " << ex.what() << " parsing options" << std::endl;
    }
    m_config = StarWin::createConfig();
    if (!m_daemon) {
        m_daemon = m_config->getBoolean(StarPaint::MAIN_GRP, DAEMON_KEY, false);
    }

    #ifdef DEBUG
    std::cout << "BackgroundApp::BackgroundApp" << std::boolalpha << m_group.m_arg_daemon << std::endl;
    #endif
}


StarWin*
BackgroundApp::createStarWindow()
{
    StarWin* starView{nullptr};
    auto builder = Gtk::Builder::create();
    const char* name = "/star-win.ui";
    //if (m_group.m_arg_daemon) {
    //    name = "/star-win2.ui";
    //}
    try {
        builder->add_from_resource(get_resource_base_path() + name);
        builder->get_widget_derived("StarWin", starView, this, m_config);
    }
    catch (const Glib::Error &ex) {
        std::cerr << "Unable to load : " << name << " error " << ex.what() << std::endl;
    }
    return starView;
}

StarWin*
BackgroundApp::getOrCreateStarWindow()
{
    // The application has been asked to open some files,
    // so let's open a new view for each one.
    StarWin* appwindow = nullptr;
    auto windows = get_windows();
    if (windows.size() > 0) {
        appwindow = dynamic_cast<StarWin*>(windows[0]);
    }
    if (!appwindow) {
        m_starAppWindow = createStarWindow();
        add_window(*m_starAppWindow);
    }
    else {
        m_starAppWindow = appwindow;
    }
    return m_starAppWindow;
}

void
BackgroundApp::on_activate()
{
    // either on_activate is called (no args)
    StarWin* imageView = getOrCreateStarWindow(); // on instance shoud be sufficent
    if (!isDaemon()) {
        imageView->set_keep_below(true);
    }
    imageView->show_all();

}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void
BackgroundApp::on_open(const Gio::Application::type_vec_files& files, const Glib::ustring& hint)
{
    // or on_open is called (some args)
    try {
        if (!files.empty()) {
            getOrCreateStarWindow();
        }
    }
    catch (const Glib::Error& ex) {
        std::cerr << "BackgroundApp::on_open() glib: " << ex.what() << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "BackgroundApp::on_open() except: " << ex.what() << std::endl;
    }
}
#pragma GCC diagnostic pop

void
BackgroundApp::on_action_quit()
{
    m_starAppWindow->hide();

    // and we shoud delete appWindow if we were not going exit anyway
    // Not really necessary, when Gtk::Widget::hide() is called, unless
    // Gio::Application::hold() has been called without a corresponding call
    // to Gio::Application::release().
    quit();
}

void
BackgroundApp::on_action_about() {
    auto builder = Gtk::Builder::create();
    try {
        builder->add_from_resource(get_resource_base_path() + "/abt-dlg.ui");
        auto dlgObj = builder->get_object("abt-dlg");
        auto dialog = Glib::RefPtr<Gtk::AboutDialog>::cast_dynamic(dlgObj);
        auto icon = Gdk::Pixbuf::create_from_resource(get_resource_base_path() + "/background.png");
        dialog->set_logo(icon);
        dialog->set_transient_for(*m_starAppWindow);
        dialog->show_all();
        dialog->run();
        dialog->hide();
    }
    catch (const Glib::Error &ex) {
        std::cerr << "Unable to load about-dialog: " << ex.what() << std::endl;
    }
}

bool
BackgroundApp::isDaemon()
{
    return m_daemon ;
}

void
BackgroundApp::on_action_help() {

}

Glib::StdStringView
BackgroundApp::get_exec_path() const
{
    return m_exec;
}

void
BackgroundApp::on_startup()
{
    #ifdef NOMAD_DEBUG
    std::cout << "BackgroundApp::on_startup" << std::endl;
    #endif
    Gtk::Application::on_startup();

   // Add actions and keyboard accelerators for the application menu.
    add_action("about", sigc::mem_fun(*this, &BackgroundApp::on_action_about));
    add_action("quit", sigc::mem_fun(*this, &BackgroundApp::on_action_quit));
    set_accel_for_action("app.quit", "<Ctrl>Q");

    if (isDaemon()) {
        auto refBuilder = Gtk::Builder::create();
        try {
            refBuilder->add_from_resource(get_resource_base_path() + "/app-menu.ui");
            auto object = refBuilder->get_object("appmenu");
            auto app_menu = Glib::RefPtr<Gio::MenuModel>::cast_dynamic(object);
            if (app_menu)
                set_app_menu(app_menu);
            else
                std::cerr << "BackgroundApp::on_startup(): No \"appmenu\" object in app_menu.ui"
                    << std::endl;
        }
        catch (const Glib::Error& ex) {
            std::cerr << "BackgroundApp::on_startup(): " << ex.what() << std::endl;
        }
    }
}

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");      // make locale dependent
    BackgroundApp app(argc, argv);

    return app.run();
}
