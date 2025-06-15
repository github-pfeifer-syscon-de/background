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

#include <KeyConfig.hpp>
#include <gtkmm.h>

#include "Layout.hpp"
#include "GeoPosition.hpp"
#include "JulianDate.hpp"
#include "SysInfo.hpp"
#include "Milkyway.hpp"
#include "Module.hpp"

class HipparcosFormat;
class ConstellationFormat;
class BackgroundApp;
class MessierLoader;
class StarWin;
class Renderer;

class StarPaint
{
public:
    StarPaint(StarWin* starWin);
    explicit StarPaint(const StarPaint& orig) = delete;
    virtual ~StarPaint() = default;

    static constexpr auto TEXT_GRAY_LOW{0.3};
    static constexpr auto TEXT_GRAY{0.6};
    static constexpr auto TEXT_GRAY_MID{0.75};
    static constexpr auto TEXT_GRAY_EMPHASIS{0.9};

    static constexpr auto LINE_WIDTH_FACTOR{900.0};
    static constexpr auto MIN_STAR_FACTOR{900.0};
    static constexpr auto MAX_STAR_FACTOR{350.0};
    static constexpr auto PLANET_FACTOR{300.0};
    static constexpr auto MESSIER_FACTOR{300.0};
    static constexpr auto CLUSTER_FACTOR{75.0};
    static constexpr auto SUNMOON_FACTOR{200.0};

    static constexpr auto START_COLOR_KEY{"startColor"};
    static constexpr auto STOP_COLOR_KEY{"stopColor"};
    static constexpr auto STAR_FONT_KEY{"starFont"};
    static constexpr auto DEFAULT_STAR_FONT{"Sans 7"};
    static constexpr auto MAIN_GRP{"main"};
    static constexpr auto SHOW_MILKYWAY_KEY{"showMilkyway"};
    static constexpr auto MESSIER_VMAGMIN_KEY{"messierVMagMin"};

    std::shared_ptr<KeyConfig> getConfig();
    Pango::FontDescription getStarFont();
    void setStarFont(const Pango::FontDescription& descr);
    Gdk::RGBA getStartColor();
    void setStartColor(const Gdk::RGBA& startColor);
    Gdk::RGBA getStopColor();
    void setStopColor(const Gdk::RGBA& stopColor);
    bool isShowMilkyway();
    void setShowMilkyway(bool showMilkyway);
    double getMessierVMagMin();
    void setMessierVMagMin(double showMessier);
    void scale(Pango::FontDescription& starFont, double scale);
    void brighten(Gdk::RGBA& calColor, double factor);
    std::vector<PtrModule> createModules();
    std::vector<PtrModule> getModules();
    std::shared_ptr<FileLoader> getFileLoader()
    {
        return m_fileLoader;
    }

    void drawImage(Cairo::RefPtr<Cairo::Context>& ctx
            , const Glib::DateTime& now
            , GeoPosition& pos
            , Layout& layout);
    void drawSky(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);

protected:
    void draw_planets(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_sun(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_moon(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_stars(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_constl(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_milkyway(Renderer* renderer ,const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    void draw_messier(Renderer* renderer, const JulianDate& jd, GeoPosition& geoPos, const Layout& layout);
    std::vector<NamedPoint> cluster(const std::vector<NamedPoint>& points, double distance = 20.0);

    std::vector<PtrModule> findModules(const char* pos);
    void drawTop(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules);
    void drawMiddle(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules);
    void drawBottom(const Cairo::RefPtr<Cairo::Context>& ctx, Layout& layout, const std::vector<PtrModule>& modules);
    double getLineWidth(const Layout& layout);
    double getSunMoonRadius(const Layout& layout);


private:
    StarWin* m_starWin;
    std::shared_ptr<HipparcosFormat> m_starFormat;
    std::shared_ptr<ConstellationFormat> m_constlFormat;
    std::shared_ptr<KeyConfig> m_config;
    std::shared_ptr<Milkyway> m_milkyway;
    std::shared_ptr<MessierLoader> m_messier;
    std::vector<PtrModule> m_modules;
    std::shared_ptr<FileLoader> m_fileLoader;
};

using PtrStarPaint = std::shared_ptr<StarPaint>;