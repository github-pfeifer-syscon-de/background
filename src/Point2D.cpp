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

#include "Point2D.hpp"

Point2D::Point2D()
: Point2D{0.0, 0.0}
{
}

Point2D::Point2D(const double _x, const double _y)
: x{_x}
, y{_y}
{
}

double
Point2D::getX() const
{
    return x;
}

double
Point2D::getY() const
{
    return y;
}

void
Point2D::setX(double _x)
{
    x = _x;
}

void
Point2D::setY(double _y)
{
    y = _y;
}

void
Point2D::add(const Point2D& add)
{
    x += add.getX();
    y += add.getY();
}

double
Point2D::getDist()
{
    return std::sqrt(x*x + y*y);
}

double
Point2D::dist(const Point2D& p)
{
    auto dx = x - p.x;
    auto dy = y - p.y;
    return std::sqrt(dx*dx + dy*dy);
}


MagPoint::MagPoint(const Point2D& p, double vMagnitude)
: Point2D{p.getX(), p.getY()}
, m_vMagnitude{vMagnitude}
{
}

double
MagPoint::getVmagnitude()
{
    return m_vMagnitude;
}


NamedPoint::NamedPoint(const MagPoint& p, const Glib::ustring& name)
: m_name{name}
{
    m_p.push_back(p);
}

Point2D
NamedPoint::getPoint() const
{
    auto m = getMagPoint();
    return Point2D(m.getX(), m.getY());
}

MagPoint
NamedPoint::getMagPoint() const
{
    return m_p[0];
}

std::vector<MagPoint>&
NamedPoint::getMagPoints()
{
    return m_p;
}

Glib::ustring
NamedPoint::getName() const
{
    return m_name;
}

void NamedPoint::add(const NamedPoint& add)
{
    if (!m_name.empty()) {
        m_name += ", ";
    }
    m_name += add.getName();
    m_p.push_back(add.getMagPoint());
}