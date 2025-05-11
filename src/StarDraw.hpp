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
#include <KeyConfig.hpp>

#include "Layout.hpp"
#include "GeoPosition.hpp"
#include "JulianDate.hpp"
#include "SysInfo.hpp"
#include "Milkyway.hpp"
#include "Module.hpp"

class HipparcosFormat;
class ConstellationFormat;
class BackgroundApp;
class StarWin;
class StarMountOp;
class MessierLoader;


class StarDraw
: public Gtk::DrawingArea
{
public:
    StarDraw(BaseObjectType* cobject
            , const Glib::RefPtr<Gtk::Builder>& builder
            , StarWin* starWin);
    explicit StarDraw(const StarDraw& orig) = delete;
    virtual ~StarDraw() = default;

    static constexpr auto TEXT_GRAY_LOW{0.3};
    static constexpr auto TEXT_GRAY{0.6};
    static constexpr auto TEXT_GRAY_MID{0.75};
    static constexpr auto TEXT_GRAY_EMPHASIS{0.9};
    static constexpr auto SUN_MOON_RADIUS{7.0};
    static constexpr auto PLANET_RADIUS{3.0};
    static constexpr auto MAX_STAR_RADIUS{3.0};
    static constexpr auto MIN_STAR_RADIUS{1.0};
    static constexpr auto MESSIER_RADIUS{4.0};
    static constexpr auto START_COLOR_KEY{"startColor"};
    static constexpr auto STOP_COLOR_KEY{"stopColor"};
    static constexpr auto STAR_FONT_KEY{"starFont"};
    static constexpr auto DEFAULT_STAR_FONT{"Sans 7"};
    static constexpr auto MAIN_GRP{"main"};
    static constexpr auto UPDATE_INTERVAL_KEY{"updateIntervalMinutes"};
    static constexpr auto SHOW_MILKYWAY_KEY{"showMilkyway"};
    static constexpr auto MESSIER_VMAGMIN_KEY{"messierVMagMin"};
    void compute();
    void update(Glib::DateTime dateTime, GeoPosition& pos);
    GeoPosition getGeoPosition();
    void setGeoPosition(const GeoPosition& geoPos);

    std::shared_ptr<KeyConfig> getConfig();
    Pango::FontDescription getStarFont();
    void setStarFont(const Pango::FontDescription& descr);
    Gdk::RGBA getStartColor();
    void setStartColor(const Gdk::RGBA& startColor);
    Gdk::RGBA getStopColor();
    void setStopColor(const Gdk::RGBA& stopColor);
    int getIntervalMinutes();
    void setIntervalMinutes(int intervalMinutes);
    bool isShowMilkyway();
    void setShowMilkyway(bool showMilkyway);
    double getMessierVMagMin();
    void setMessierVMagMin(double showMessier);
    StarWin* getWindow();
    void saveConfig();
    void scale(Pango::FontDescription& starFont, double scale);
    void brighten(Gdk::RGBA& calColor, double factor);

    std::shared_ptr<InfoModule> getInfoModule()
    {
        return m_infoModule;
    }
    std::shared_ptr<ClockModule> getClockModule()
    {
        return m_clockModule;
    }
    std::shared_ptr<CalendarModule> getCalendarModule()
    {
        return m_calendarModule;
    }

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

    void draw_planets(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_sun(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_moon(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_stars(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_constl(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void drawSky(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_milkyway(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_messier(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    std::vector<NamedPoint> cluster(const std::vector<NamedPoint>& points, double distance = 20.0);

    std::string getGlobeConfigName();
    void setupConfig();

    bool on_button_press_event(GdkEventButton* event) override;
    Gtk::Menu *build_popup();
    void on_menu_param();

    std::vector<PtrModule> findModules(const char* pos);
    void drawTop(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules);
    void drawMiddle(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules);
    void drawBottom(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules);

private:
    std::shared_ptr<HipparcosFormat> m_starFormat;
    std::shared_ptr<ConstellationFormat> m_constlFormat;
    GeoPosition m_geoPos;
    std::shared_ptr<KeyConfig> m_config;
    static constexpr auto GRP_GLGLOBE_MAIN{"globe"};
    static constexpr auto LATITUDE_KEY{"lat"};
    static constexpr auto LONGITUDE_KEY{"lon"};
    Cairo::RefPtr<Cairo::ImageSurface> m_image;
    std::shared_ptr<Milkyway> m_milkyway;
    std::shared_ptr<MessierLoader> m_messier;
    bool m_updateBlocked{false};
    Glib::DateTime m_displayTimeUtc;
    StarWin* m_starWin;
    std::shared_ptr<InfoModule> m_infoModule;
    std::shared_ptr<ClockModule> m_clockModule;
    std::shared_ptr<CalendarModule> m_calendarModule;
};
