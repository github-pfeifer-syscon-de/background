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

#include "Layout.hpp"
#include "GeoPosition.hpp"
#include "JulianDate.hpp"
#include "SysInfo.hpp"
#include "Mercury.hpp"
#include "Venus.hpp"
#include "Mars.hpp"
#include "Jupiter.hpp"
#include "Saturn.hpp"
#include "Uranus.hpp"
#include "Neptune.hpp"
#include "Milkyway.hpp"

class NetConnection;
class HipparcosFormat;
class ConstellationFormat;
class BackgroundApp;

class StarDraw
: public Gtk::DrawingArea
{
public:
    StarDraw(BaseObjectType* cobject
            , const Glib::RefPtr<Gtk::Builder>& builder
            , BackgroundApp& appl);
    explicit StarDraw(const StarDraw& orig) = delete;
    virtual ~StarDraw() = default;

    static constexpr auto TEXT_GRAY = 0.6;
    static constexpr auto TEXT_GRAY_EMPHASIS = 0.9;
    static constexpr auto SUN_MOON_RADIUS = 7.0;
    static constexpr auto PLANET_READIUS = 3.0;

    void compute();
    void update(Glib::DateTime dateTime, GeoPosition& pos);
    GeoPosition getGeoPosition();
protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

    void drawInfo(const Cairo::RefPtr<Cairo::Context>& ctx, double size);
    void drawClock(const Cairo::RefPtr<Cairo::Context>& ctx, double radius);
    void drawCalendar(const Cairo::RefPtr<Cairo::Context>& ctx, double size, const Layout& layout);

    void draw_planets(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_sun(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_moon(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_stars(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_constl(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void drawSky(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_milkyway(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);

    const std::array<std::shared_ptr<Planet>,7> planets =
                        {std::make_shared<Mercury>(),
                        std::make_shared<Venus>(),
                        std::make_shared<Mars>(),
                        std::make_shared<Jupiter>(),
                        std::make_shared<Saturn>(),
                        std::make_shared<Uranus>(),
                        std::make_shared<Neptune>()};

    std::string get_config_name();
    void setupConfig();

    bool on_button_press_event(GdkEventButton* event) override;
    Gtk::Menu *build_popup();
    void on_menu_param();
    void saveConfig();
private:
    std::shared_ptr<HipparcosFormat> m_starFormat;
    std::shared_ptr<ConstellationFormat> m_constlFormat;
    GeoPosition m_geoPos;

    static constexpr auto GRP_MAIN{"globe"};
    static constexpr auto LATITUDE{"lat"};
    static constexpr auto LONGITUDE{"lon"};
    Cairo::RefPtr<Cairo::ImageSurface> m_image;
    std::shared_ptr<Milkyway> m_milkyway;
    bool m_updateBlocked{false};
    Glib::DateTime m_displayTimeUtc;
    BackgroundApp& m_appl;
};

