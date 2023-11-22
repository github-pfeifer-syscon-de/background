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
#include <list>
#include <memory>

#include "Poly.hpp"

class Milkyway
{
public:
    Milkyway(const Gtk::Application& appl);
    explicit Milkyway(const Milkyway& orig) = delete;
    virtual ~Milkyway() = default;

    std::list<std::shared_ptr<Poly>> getBounds();
protected:
    void readFeature(JsonObject* feature, JsonHelper& jsonHelper, std::list<std::shared_ptr<Poly>>& polys);

    std::list<std::shared_ptr<Poly>> readBounds();
private:
    const Gtk::Application& m_appl;
    std::list<std::shared_ptr<Poly>> m_bounds;
};

