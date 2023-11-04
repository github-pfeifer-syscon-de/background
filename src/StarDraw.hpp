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

class HipparcosFormat;
class ConstellationFormat;

class StarDraw
: public Gtk::DrawingArea
{
public:
    StarDraw(BaseObjectType* cobject
            , const Glib::RefPtr<Gtk::Builder>& builder
            , const Gtk::Application& appl);
    explicit StarDraw(const StarDraw& orig) = delete;
    virtual ~StarDraw() = default;

    static constexpr auto SUN_MOON_RADIUS = 7.0;
protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

    void draw_constl(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const GeoPosition& geoPos, const Layout& layout);
    void draw_stars(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const GeoPosition& geoPos, const Layout& layout);
    void draw_moon(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const GeoPosition& geoPos, const Layout& layout);
    void draw_sun(const Cairo::RefPtr<Cairo::Context>& ctx, const JulianDate& jd, const GeoPosition& geoPos, const Layout& layout);

    std::string get_config_name();
    void setupConfig();

private:
    std::shared_ptr<HipparcosFormat> m_starFormat;
    std::shared_ptr<ConstellationFormat> m_constlFormat;
    GeoPosition m_geoPos;

    static constexpr auto GRP_MAIN{"globe"};
    static constexpr auto LATITUDE{"lat"};
    static constexpr auto LONGITUDE{"lon"};
};

