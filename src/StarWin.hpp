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

#include "GeoPosition.hpp"
#include "config.h"

class StarDraw;
class BackgroundApp;
class StarMountOp;
class AppMenu;
class StarPaint;
class KeyConfig;
class FileLoader;

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
    void do_close();
    void loadConfig();
    void saveConfig();
    void savePosition();
    std::shared_ptr<KeyConfig> getConfig()
    {
        return m_config;
    }
    std::shared_ptr<StarPaint> getStarPaint()
    {
        return m_starPaint;
    }
    std::shared_ptr<FileLoader> getFileLoader()
    {
        return m_fileLoader;
    }
    int getIntervalMinutes();
    void setIntervalMinutes(int intervalMinutes);
    GeoPosition getGeoPosition();
    void setGeoPosition(const GeoPosition& geoPos);
    void update();
    void update(Glib::DateTime dateTime, GeoPosition& pos);
    void on_menu_param();
    void on_menu_time();

protected:
    std::string getGlobeConfigName();
    void setupConfig();
    bool updatePeriodic();
    void updateTimer();
    void cancel();
    void on_mount(Glib::RefPtr<Gio::AsyncResult>& result);
    void on_eject(Glib::RefPtr<Gio::AsyncResult>& result);
    void cleanUp(Glib::RefPtr<Gio::File>&dir, const std::string& keepName);
    void exportPdf();
    static constexpr auto IMAGE_PREFIX{"starDesk_"};
    static constexpr auto CONFIG_NAME{"background.conf"};
    static constexpr auto DESKTOP_BACKGR_IMAGE{"$img"};
    static constexpr auto DESKTOP_BACKGR_KEY{"desktopBackground"};
    static constexpr auto UPDATE_INTERVAL_KEY{"updateIntervalMinutes"};
    static constexpr auto GRP_GLGLOBE_MAIN{"globe"};
    static constexpr auto LATITUDE_KEY{"lat"};
    static constexpr auto LONGITUDE_KEY{"lon"};

private:
    StarDraw* m_drawingArea{nullptr};
    std::shared_ptr<FileLoader> m_fileLoader;
    sigc::connection m_timer;
    sigc::connection m_timerUpdate;
    BackgroundApp* m_backAppl;
    std::shared_ptr<KeyConfig> m_config;
    GeoPosition m_geoPos;

    Glib::RefPtr<Gio::VolumeMonitor> m_volumeMonitor;
    Glib::RefPtr<Gio::Cancellable> m_cancelable;
    Glib::RefPtr<StarMountOp> m_starMountOp;
    Glib::RefPtr<Gio::Volume> m_activeVolume;   // e.g. the volume we are currently working on opening, ejecting...
    GPid m_pid;
    std::shared_ptr<StarPaint> m_starPaint;
    bool m_updateBlocked{false};

#   ifdef USE_APPMENU
    std::shared_ptr<AppMenu> m_appMenu;
#   endif
};

