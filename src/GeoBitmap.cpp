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

#include <iostream>

#include "GeoBitmap.hpp"


double
GeoBitmap::getPixelPerLongitude()
{
    if (m_pixelPerLongitude == 0.0) {
        m_pixelPerLongitude = static_cast<double>(m_pixmap->get_width()) / (m_max.getLongitude() - m_min.getLongitude());
    }
    return m_pixelPerLongitude;
}

double
GeoBitmap::getPixelPerLatitude()
{
    if (m_pixelPerLatitude == 0.0) {
        m_pixelPerLatitude = static_cast<double>(m_pixmap->get_height()) / (m_max.getLatitude() - m_min.getLatitude());
    }
    return m_pixelPerLatitude;
}

[[nodiscard]] auto
GeoBitmap::getSlice(const GeoCoord& coord, const GeoCoord& size)
->Glib::RefPtr<Gdk::Pixbuf>
{
    int xImg = static_cast<int>((coord.getLongitude() - m_min.getLongitude()) * getPixelPerLongitude());
    // for image start from top
    int yImg = static_cast<int>((m_max.getLatitude() - (coord.getLatitude() + size.getLatitude())) * getPixelPerLatitude());
    int imgWidth = static_cast<int>(size.getLongitude() * getPixelPerLongitude());
    int imgHeight = static_cast<int>(size.getLatitude() * getPixelPerLatitude());
    //std::cout << "GeoBitmap::getSlice "
    //    << " long = " << coord.longitude << " xImg " << xImg << " imgWidth " << imgWidth << "\n"
    //    << " lati = " << coord.latitude << " yImg " << yImg << " imgHeight " << imgHeight << std::endl;
    auto target = Gdk::Pixbuf::create(m_pixmap->get_colorspace(), m_pixmap->get_has_alpha(), m_pixmap->get_bits_per_sample(),
        imgWidth, imgHeight);
    m_pixmap->copy_area(xImg, yImg, imgWidth, imgHeight, target, 0, 0);
    return target;
}
