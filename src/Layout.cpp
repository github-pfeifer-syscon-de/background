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

#include "Layout.hpp"


Layout::Layout(int _width, int _height)
: m_width{_width}
, m_height{_height}
{
}

int
Layout::getWidth() const
{
    return m_width;
}

int
Layout::getHeight() const
{
    return m_height;
}

int
Layout::getMin() const
{
    return std::min(m_width, m_height);
}

int Layout::getXOffs() const
{
    return m_xOffs;
}

void Layout::setXOffs(int xOffs)
{
    m_xOffs = xOffs;
}

int
Layout::getYOffs() const
{
    return m_yOffs;
}

void
Layout::setYOffs(int yOffs)
{
    m_yOffs = yOffs;
}