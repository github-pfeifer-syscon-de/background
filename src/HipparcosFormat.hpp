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
#include <vector>
#include <memory>
#include <string>
#include <JsonHelper.hpp>

#include "Star.hpp"
#include "HipparcosStar.hpp"

class Field {
public:
    /**
     *  "name": "HIP",
     *   "description": "Hipparcos identifier",
     *   "datatype": "INT",
     *   "ucd": "meta.id;meta.main"
     */
    Field(JsonObject* metadata, int _index) {
        name = json_object_get_string_member(metadata, "name");
        index = _index;
    }

    std::string getName() {
	return name;
    }

    double getDouble(JsonArray* array) {
        return json_array_get_double_element(array, index);
    }

    std::string getString(JsonArray* array) {
        return json_array_get_string_element(array, index);
    }

    gint64 getLong(JsonArray* array) {
        return json_array_get_int_element(array, index);
    }
private:
    std::string name;
    int index{0};
};


class HipparcosFormat
{
public:
    HipparcosFormat(const Gtk::Application& appl);
    explicit HipparcosFormat(const HipparcosFormat& orig) = delete;
    virtual ~HipparcosFormat() = default;

    std::vector<std::shared_ptr<Star>> getStars();


private:
    std::vector<std::shared_ptr<HipparcosStar>> readStars();
    std::vector<std::shared_ptr<HipparcosStar>> stars;
    const Gtk::Application& m_appl;
};

