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
#include <StringUtils.hpp>
#include <psc_format.hpp>

#include "StarWin.hpp"
#include "StarDraw.hpp"
#include "BackgroundApp.hpp"
#include "StarMountOperation.hpp"
#include "FileLoader.hpp"
#include "StarPaint.hpp"
#include "KeyConfig.hpp"
#include "ParamDlg.hpp"
#include "TimeDlg.hpp"
#include "FileLoader.hpp"
#ifdef USE_APPMENU
#include "AppMenu.hpp"
#endif
#include <ImageFileChooser.hpp>
#ifdef USE_PDF
#include <HaruRenderer.hpp>
#endif
#include <Renderer.hpp>

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

    setupConfig();
    m_fileLoader = std::make_shared<FileLoader>(backAppl->get_exec_path());
    m_starPaint = std::make_shared<StarPaint>(this);
    if (m_backAppl->isDaemon()) {
        iconify();
        add_action("preferences", sigc::mem_fun(*this, &StarWin::on_menu_param));
        add_action("time", sigc::mem_fun(*this, &StarWin::on_menu_time));
#       ifdef USE_PDF
        add_action("export", sigc::mem_fun(*this, &StarWin::exportPdf));
#       endif
        update();
    }
    else {
        builder->get_widget_derived("drawingArea", m_drawingArea, this);
        set_decorated(false);
        maximize();
#       ifdef USE_APPMENU
        m_appMenu = std::make_shared<AppMenu>();
#       endif
    }
    updateTimer();
    signal_hide().connect([this] {
        if (m_timer.connected()) {
            m_timer.disconnect();
        }
    });
}


void
StarWin::loadConfig()
{
    if (!m_config) {
        m_config = std::make_shared<KeyConfig>(CONFIG_NAME);
    }
    else {
        try {
            m_config->getConfig()->load_from_file(m_config->getConfigName());
        }
        catch (const Glib::FileError& exc) {
            std::cerr << "Cound not read " << exc.what() << " config " << CONFIG_NAME << " (it may not yet exist and will be created)." << std::endl;
        }
    }
}

void
StarWin::setupConfig()
{
    loadConfig();
    std::string cfg = getGlobeConfigName();
    // since it is more convenient to use the location we saved for glglobe load&save it from there
    //   but this keeps the risk of overwriting the coordinates (if you change them on both sides...)
    try {
        auto cfgFile = Gio::File::create_for_path(cfg);
        if (!cfgFile->query_exists()) {
            Glib::ustring msg("No config found, please enter your position.");
            showMessage(msg);
            on_menu_time();
        }
        else {
            auto config = std::make_shared<Glib::KeyFile>();
            if (config->load_from_file(cfg, Glib::KEY_FILE_NONE)
             && config->has_group(GRP_GLGLOBE_MAIN)) {
                if (config->has_key(GRP_GLGLOBE_MAIN, LATITUDE_KEY))
                    m_geoPos.setLatDegrees(config->get_double(GRP_GLGLOBE_MAIN, LATITUDE_KEY));
                if (config->has_key(GRP_GLGLOBE_MAIN, LONGITUDE_KEY))
                    m_geoPos.setLonDegrees(config->get_double(GRP_GLGLOBE_MAIN, LONGITUDE_KEY));
            }
        }
    }
    catch (const Glib::Error &ex) {
        auto msg = Glib::ustring::sprintf("Error %s loading %s", ex.what(), cfg);
        showMessage(msg, Gtk::MessageType::MESSAGE_ERROR);
    }
}

std::string
StarWin::getGlobeConfigName()
{
    // share config with glglobe as we already have coordinates
    auto fullPath = Glib::canonicalize_filename("glglobe.conf", Glib::get_user_config_dir());
    //std::cout << "using config " << fullPath << std::endl;
    return fullPath;
}

void
StarWin::saveConfig()
{
    try {
        m_config->saveConfig();
    }
    catch (const Glib::Error &ex) {
        auto msg = Glib::ustring::sprintf("Error %s saving config", ex.what());
        showMessage(msg, Gtk::MessageType::MESSAGE_ERROR);
    }
}

void
StarWin::savePosition()
{
    // handle glglobe&background config
    std::string cfg = getGlobeConfigName();
    try {
        auto config = std::make_shared<Glib::KeyFile>();
        auto cfgFile = Gio::File::create_for_path(cfg);
        if (cfgFile->query_exists()) {     // do load as file may contains other stuff
            config->load_from_file(cfg, Glib::KEY_FILE_NONE);
        }
        config->set_double(GRP_GLGLOBE_MAIN, LATITUDE_KEY, m_geoPos.getLatDegrees());
        config->set_double(GRP_GLGLOBE_MAIN, LONGITUDE_KEY, m_geoPos.getLonDegrees());
        config->save_to_file(cfg);
    }
    catch (const Glib::Error &ex) {
        auto msg = Glib::ustring::sprintf("Error %s saving %s", ex.what(), cfg);
        showMessage(msg, Gtk::MessageType::MESSAGE_ERROR);
    }
}

void
StarWin::on_menu_param()
{
	ParamDlg::show(this);
    update();
}

void
StarWin::on_menu_time()
{
    m_updateBlocked = true;
	TimeDlg::show(this);
    m_updateBlocked = false;
    update();      // reset to default view
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
    uint32_t intervalMinutes = getIntervalMinutes();
    auto delay = (intervalMinutes * 60000u + 20u) - static_cast<unsigned int>(dateTime.get_seconds() * 1000.0);
    m_timer = Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &StarWin::updatePeriodic)
            , delay);
}

bool
StarWin::updatePeriodic()
{
    if (!m_updateBlocked) {
        auto now = Glib::DateTime::create_now_utc();
        auto pos = getGeoPosition();
        update(now, pos);
    }
    updateTimer();
    return false;
}

void
StarWin::update()
{
    if (m_timerUpdate.connected()) {
        m_timerUpdate.disconnect();
    }
    // delay updating in case of rapid config changes
    m_timerUpdate = Glib::signal_timeout().connect(
        [this] {
            auto now = Glib::DateTime::create_now_utc();
            auto pos = getGeoPosition();
            update(now, pos);
            return false;
        }, 100);
}

void
StarWin::update(Glib::DateTime now, GeoPosition& pos)
{
    if (m_backAppl->isDaemon()) {
        auto screen = Gdk::Screen::get_default();
        int width = screen->get_width();
        int height = screen->get_height();
        auto image = Cairo::ImageSurface::create(Cairo::Format::FORMAT_ARGB32, width, height);
        Layout layout(width, height);
        auto ctx = Cairo::Context::create(image);
        m_starPaint->drawImage(ctx, now, pos, layout);
        // create new
        auto dateTime = now.format("%F_%H%M%S%f");  // build a long name, as updates work only when filename changes e.g. from settings dialog
        auto fileName = psc::fmt::format("{}{}.png", IMAGE_PREFIX, dateTime);
        auto localDir = m_fileLoader->getLocalDir();
        auto temp = localDir->get_child(fileName);
        //std::cout << "Temp " << temp->get_path() << std::endl;
        image->write_to_png(temp->get_path());
        std::string cmd = m_config->getString(StarPaint::MAIN_GRP, DESKTOP_BACKGR_KEY);
        auto cmds = StringUtils::splitConsec(cmd, ' ');
        if (cmds.empty()) {
            cmds.push_back("/usr/bin/xfconf-query");
            cmds.push_back("-c");
            cmds.push_back("xfce4-desktop");
            cmds.push_back("-p");
            cmds.push_back("/backdrop/screen0/monitor1/workspace0/last-image");
            cmds.push_back("-s");
            cmds.push_back(DESKTOP_BACKGR_IMAGE);
            std::function<std::string(const std::string& item)> lambda =
                [] (const std::string& item) -> auto
                {
                    return item;
                };
            cmd = StringUtils::concat(cmds, std::string(" "), lambda);
            m_config->setString(StarPaint::MAIN_GRP, DESKTOP_BACKGR_KEY, cmd);
            saveConfig();
            showMessage(Glib::ustring::sprintf("A config to change the desktop background was not found, a default for Xfce was created you need to adapt it most likely (see %s).", CONFIG_NAME));
        }
        for (uint32_t i = 0; i < cmds.size(); ++i) {
            if (cmds[i] == DESKTOP_BACKGR_IMAGE) {
                cmds[i] = temp->get_path();
            }
        }
        GPid pid;
        m_fileLoader->run(cmds, &pid);
        cleanUp(localDir, fileName);
    }
    else {
        m_drawingArea->update(now, pos);
    }
}

// remove any leftover files
void
StarWin::cleanUp(Glib::RefPtr<Gio::File>& dir, const std::string& keepName)
{
    auto cancel = Gio::Cancellable::create();
    auto enumer = dir->enumerate_children(cancel);
    while (true) {
        auto fileInfo = enumer->next_file();
        if (!fileInfo) {
            break;
        }
        if (fileInfo->get_file_type() == Gio::FileType::FILE_TYPE_REGULAR
         && StringUtils::startsWith(fileInfo->get_name(), IMAGE_PREFIX)
         && StringUtils::endsWith(fileInfo->get_name(), ".png")
         && fileInfo->get_name() != keepName) {
            auto file = dir->get_child(fileInfo->get_name());
            file->remove();
        }
    }
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
                auto msg = m_starPaint->getFileLoader()->run(args, &m_pid);
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

	auto mparam = Gtk::make_managed<Gtk::MenuItem>("_Parameter", true);
	mparam->signal_activate().connect(sigc::mem_fun(*this, &StarWin::on_menu_param));
	pMenuPopup->append(*mparam);

	auto mtime = Gtk::make_managed<Gtk::MenuItem>("_Timeshift&Position", true);
	mtime->signal_activate().connect(sigc::mem_fun(*this, &StarWin::on_menu_time));
	pMenuPopup->append(*mtime);


	auto mabout = Gtk::make_managed<Gtk::MenuItem>("_About", true);
	mabout->signal_activate().connect(sigc::mem_fun(*m_backAppl, &BackgroundApp::on_action_about));
	pMenuPopup->append(*mabout);


#   ifdef USE_APPMENU
	auto mapps = Gtk::make_managed<Gtk::MenuItem>("_Apps", true);
    mapps->signal_activate().connect(
        sigc::bind(
              sigc::mem_fun(*m_appMenu.get(), &AppMenu::on_menu_show)
            , this));
    pMenuPopup->append(*mapps);
	//m_appMenu->addMenu(pMenuPopup);
#   endif

	auto mclose = Gtk::make_managed<Gtk::MenuItem>("_Close", true);
	mclose->signal_activate().connect(sigc::mem_fun(*this, &StarWin::do_close));
	pMenuPopup->append(*mclose);

    //auto mounts = m_volumeMonitor->get_mounts();
    //for (auto mount : mounts) {
    //    std::cout << "Mount " << mount->get_name() << std::endl;
    //}
}

void
StarWin::do_close()
{
    hide(); // terminates app as well
}

void
StarWin::showMessage(const Glib::ustring& msg, Gtk::MessageType msgType)
{
    Gtk::MessageDialog messagedialog(*this, msg, false, msgType);
    messagedialog.run();
    messagedialog.hide();
}

GeoPosition
StarWin::getGeoPosition()
{
    return m_geoPos;
}

void
StarWin::setGeoPosition(const GeoPosition& geoPos)
{
    m_geoPos = geoPos;
}

int
StarWin::getIntervalMinutes()
{
    return m_config->getInteger(StarPaint::MAIN_GRP, UPDATE_INTERVAL_KEY, 1);
}

void
StarWin::setIntervalMinutes(int intervalMinutes)
{
    return m_config->setInteger(StarPaint::MAIN_GRP, UPDATE_INTERVAL_KEY, intervalMinutes);
}

void
StarWin::exportPdf()
{
#   ifdef USE_PDF
    auto now = Glib::DateTime::create_now_utc();
    auto pos = getGeoPosition();
    auto dateTime = now.to_local().format("%F_%H:%M");
    auto info = Glib::ustring::sprintf("%s lon %.1lf° lat %.1lf°", dateTime, pos.getLonDegrees(), pos.getLatDegrees());

    JulianDate jd(now);
    try {
        HaruRenderer haruRenderer;
        Layout layout = haruRenderer.getLayout();
        auto screen = get_screen();
        auto ref = std::min(screen->get_height(), screen->get_width());
        // this is just a guess, since the display may appear somewhere else, but we can't tell
        haruRenderer.setReference(ref);
        auto starFont = m_starPaint->getStarFont();
        m_starPaint->scale(starFont, 1.5);
        auto infoTxt = haruRenderer.createText(starFont);
        infoTxt->setText(info);
        //std::cout << "layout " << layout.getWidth() << " height " << layout.getHeight() << std::endl;
        //std::cout << "info " << info << " xOffs " << layout.getXOffs() << " height " << layout.getHeight() - layout.getYOffs() << std::endl;
        haruRenderer.showText(infoTxt, layout.getXOffs(), layout.getYOffs() + (layout.getHeight() -  layout.getMin()) / 2.0, TextAlign::LeftBottom);
        haruRenderer.setInvertY(true);      // as we work with cairo coordinates from here on
        m_starPaint->drawSky(&haruRenderer, jd, pos, layout);
        ImageFileChooser file_chooser(*this, true, {"pdf"});
        if (file_chooser.run() == Gtk::ResponseType::RESPONSE_ACCEPT) {
            haruRenderer.save(file_chooser.get_file()->get_path());
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Error exporting " << ex.what() << std::endl;
    }
    catch (...) {
        std::cout << "Any error when exporting!" << std::endl;
    }

#   endif
}