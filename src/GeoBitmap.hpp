/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2026 RPf
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

#include <glibmm.h>
#include <gdkmm/pixbuf.h>

#include "GeoConversion.hpp"

class GeoBitmap {
public:
    GeoBitmap() = default;
    explicit GeoBitmap(const GeoBitmap& other) = delete;
    virtual ~GeoBitmap() = default;
    void setMinimum(const GeoCoord& min) {
        m_min = min;
    }
    void setMaximum(const GeoCoord& max) {
        m_max = max;
    }
    void setPixmap(const Glib::RefPtr<Gdk::Pixbuf>& pixmap) {
        m_pixmap = pixmap;
    }
    [[nodiscard]] auto getPixmap()
    ->Glib::RefPtr<Gdk::Pixbuf> {
        return m_pixmap;
    }
    /**
     * Get a slice for coord & size from a bigger pixelmap.
     * warn: no handling for partial overlap.
     **/
    [[nodiscard]] auto getSlice(const GeoCoord& coord, const GeoCoord& size)
    ->Glib::RefPtr<Gdk::Pixbuf>;
protected:
    double getPixelPerLongitude();
    double getPixelPerLatitude();

private:
    GeoCoord m_min;
    GeoCoord m_max;
    Glib::RefPtr<Gdk::Pixbuf> m_pixmap;
    double m_pixelPerLongitude{};
    double m_pixelPerLatitude{};

};
