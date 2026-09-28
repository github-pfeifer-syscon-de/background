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

#include <GeoJson.hpp>
#include <GeoCoordinate.hpp>


class GeoJsonPoint
{
public:
    GeoJsonPoint() = default;
    virtual ~GeoJsonPoint() = default;

    GeoCoordinate getPosition(){
        return m_geoPosition;
    }
    void setPosition(GeoCoordinate& geoPos) {
        m_geoPosition = geoPos;
    }
    Glib::ustring getName() {
        return m_name;
    }
    void setName(const Glib::ustring& name){
        m_name = name;
    }
    int32_t getPopulation() {
        return m_population;
    }
    void setPopulation(int population) {
        m_population = population;
    }
private:
    GeoCoordinate m_geoPosition;
    Glib::ustring m_name;
    int32_t m_population;
};

using PtrGeoJsonPoint= std::shared_ptr<GeoJsonPoint>;

class GeoPointHandler
: public GeoJsonHandler
{
public:
    GeoPointHandler();
    explicit GeoPointHandler(const GeoPointHandler& other) = delete;
    virtual ~GeoPointHandler() = default;

    void addFeature(JsonObject*) override;
    void endFeature() override;
    void addGeometry(JsonObject*) override;
    void endGeometry() override;
    void addMultiPolygon(JsonArray* ) override;
    void endMultiPolygon() override;
    void addPolygon(JsonArray*) override;
    void endPolygon() override;
    void addShape(JsonArray*) override;
    void endShape() override;
    void addCoord(JsonArray*, bool) override;
    void addPoint(JsonArray*) override;
    void addProperties(JsonObject*) override;

    const std::vector<PtrGeoJsonPoint>& getPoints()
    {
        return m_points;
    }

private:
    PtrGeoJsonPoint m_point;
    std::vector<PtrGeoJsonPoint> m_points;
};

using PtrGeoPointHandler = std::shared_ptr<GeoPointHandler>;
