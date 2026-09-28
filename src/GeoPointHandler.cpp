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
#include "GeoPointHandler.hpp"

#include <iostream>

GeoPointHandler::GeoPointHandler()
{
    m_points.reserve(256);
}

void
GeoPointHandler::addFeature(JsonObject* obj)
{
    m_point = std::make_shared<GeoJsonPoint>();
    //std::cout << "GeoPointHandler::addFeature"
    //          << " obj size " << json_object_get_size(obj) << std::endl;
}

void
GeoPointHandler::endFeature()
{
    if (m_point) {
        m_points.emplace_back(std::move(m_point));
    }
    //std::cout << "GeoPointHandler::endFeature" << std::endl;
}

void
GeoPointHandler::addGeometry(JsonObject* obj)
{
    //std::cout << "GeoPointHandler::addGeometry"
    //          << " obj size " << json_object_get_size(obj) << std::endl;
}

void
GeoPointHandler::endGeometry()
{
    //std::cout << "GeoPointHandler::endGeometry" << std::endl;
}

void
GeoPointHandler::addMultiPolygon(JsonArray* arr)
{
    //std::cout << "GeoPointHandler::addMultiPolygon"
    //          << " arr size " << json_array_get_length(arr) << std::endl;
}

void
GeoPointHandler::endMultiPolygon()
{
    //std::cout << "GeoPointHandler::endMultiPolygon" << std::endl;
}

void
GeoPointHandler::addPolygon(JsonArray* arr)
{
    //std::cout << "GeoPointHandler::addPolygon"
    //          << " arr size " << json_array_get_length(arr) << std::endl;
}

void
GeoPointHandler::endPolygon()
{
    //std::cout << "GeoPointHandler::endPolygon" << std::endl;
}

void
GeoPointHandler::addShape(JsonArray* arr)
{
    //std::cout << "GeoPointHandler::addShape"
    //          << " arr size " << json_array_get_length(arr) << std::endl;
}

void
GeoPointHandler::endShape()
{
    //std::cout << "GeoPointHandler::endShape" << std::endl;
}

void
GeoPointHandler::addCoord(JsonArray* arr, bool boolVal)
{
    //std::cout << "GeoPointHandler::addCoord"
    //          << " arr size " << json_array_get_length(arr)
    //          << " bool " << std::boolalpha << boolVal << std::endl;
}

void
GeoPointHandler::addPoint(JsonArray* arr)
{
    //std::cout << "GeoPointHandler::addPoint"
    //          << " arr size " << json_array_get_length(arr) << std::endl;
    int size = json_array_get_length(arr);
    if (m_point && size >= 2) {
        double lon = json_array_get_double_element(arr, 0);
        double lat = json_array_get_double_element(arr, 1);
        GeoCoordinate pos{lon, lat, CoordRefSystem::CRS_84};
        m_point->setPosition(pos);
    }
}

void
GeoPointHandler::addProperties(JsonObject* obj)
{
    if (static_cast<bool>(json_object_has_member(obj, "name"))) {
        Glib::ustring name = json_object_get_string_member(obj, "name");
        m_point->setName(name);
        //std::cout << "   name " << name << std::endl;
    }
    if (static_cast<bool>(json_object_has_member(obj, "population"))) {
        int32_t population = static_cast<int32_t>(json_object_get_int_member(obj, "population"));
        m_point->setPopulation(population);
        //std::cout << "   population " << population << std::endl;
    }
}
