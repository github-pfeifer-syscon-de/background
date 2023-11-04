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