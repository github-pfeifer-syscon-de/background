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

#include <iostream>

#include "Milkyway.hpp"

// descr https://astronomy.stackexchange.com/questions/18229/milky-way-position-on-the-sky
Milkyway::Milkyway(const Gtk::Application& appl)
: m_appl{appl}
{

}

std::list<std::shared_ptr<Poly>>
Milkyway::getBounds()
{
    if (m_bounds.empty()) {
        m_bounds = readBounds();
    }
    return m_bounds;	//??? give away internal structure
}

void
Milkyway::readFeature(JsonObject* feature, JsonHelper& jsonHelper, std::list<std::shared_ptr<Poly>>& polys)
{
    Glib::ustring id = json_object_get_string_member(feature, "id");
    auto ints = id.substr(2);   // e.g. "ol1" -> intensity 1
    auto intensity = std::stoi(ints);
    JsonObject* geometry = jsonHelper.get_object(feature, "geometry");
    JsonArray* coords = jsonHelper.get_array(geometry, "coordinates");
    uint32_t poliesCount = json_array_get_length(coords);
    //std::cout << __FILE__ << "::read polies " << poliesCount << std::endl;
    for (uint32_t nPolies = 0; nPolies < poliesCount; ++nPolies) {
        JsonArray* multipolies =json_array_get_array_element(coords, nPolies) ;
        uint32_t polyCount = json_array_get_length(multipolies);
        //std::cout << "  " << __FILE__ << "::read poly " << polyCount << std::endl;
        for (uint32_t nPoly = 0; nPoly < polyCount; ++nPoly) {
            JsonArray* japoly = json_array_get_array_element(multipolies, nPoly);
            auto poly = std::make_shared<Poly>();
            poly->setIntensity(intensity);
            poly->read(japoly);
            polys.push_back(poly);
        }
    }
}


std::list<std::shared_ptr<Poly>>
Milkyway::readBounds()
{
    std::list<std::shared_ptr<Poly>> polys;
    JsonHelper jsonHelper;
    auto resource_path = m_appl.get_resource_base_path() + "/mw.json";
    auto bytes = Gio::Resource::lookup_data_global(resource_path);
    gsize size{0};
    auto data = bytes->get_data(size);
    if (data != nullptr && size > 0u) {
        try {
            auto array = Glib::ByteArray::create();
            array->append(static_cast<const guint8*>(data), static_cast<guint>(size));
            jsonHelper.load_data(array);
            JsonObject* root = jsonHelper.get_root_object();
            JsonArray* features = jsonHelper.get_array(root, "features");
            for (uint32_t m = 0; m < json_array_get_length(features); ++m) {
                JsonObject* feature = jsonHelper.get_array_object(features, m);
                readFeature(feature, jsonHelper, polys);
            }
        }
        catch (const JsonException& exc) {
            std::cout << "The milkyway data was not be loaded exception " << exc.what() << "!" << std::endl;
        }
    }
    else {
        std::cout << "The milkyway data " << resource_path << " was not found!" << std::endl;
    }
    return polys;
}
