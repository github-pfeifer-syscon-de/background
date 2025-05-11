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

#include <vector>
#include <glibmm.h>

class Point2D
{
public:
    Point2D();
    Point2D(const double _x, const double _y);
    explicit Point2D(const Point2D& orig) = default;
    virtual ~Point2D() = default;

    double getX() const;
    double getY() const;
    void setX(double x);
    void setY(double y);
    void add(const Point2D& add);
    double getDist();
    double dist(const Point2D& p);
private:
    double x;
    double y;
};

class MagPoint
: public Point2D
{
public:
    MagPoint(const Point2D& p, double vMagnitude);
    MagPoint(const MagPoint& p) = default;
    virtual ~MagPoint() = default;

    double getVmagnitude();
private:
    double m_vMagnitude;
};

class NamedPoint
{
public:
    NamedPoint(const MagPoint& p, const Glib::ustring& name);
    NamedPoint(const NamedPoint& p) = default;
    virtual ~NamedPoint() = default;

    Point2D getPoint() const;
    MagPoint getMagPoint() const;
    std::vector<MagPoint>& getMagPoints();
    Glib::ustring getName() const;
    void add(const NamedPoint& add);
private:
    std::vector<MagPoint> m_p;
    Glib::ustring m_name;
};