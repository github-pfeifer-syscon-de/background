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

#include <cmath>
#include <iterator>

struct GeoCoord {
public:
    GeoCoord()
    : longitude{}
    , latitude{}
    {
    }
    explicit GeoCoord(double offs)
    : GeoCoord{offs, offs}
    {
    }
    GeoCoord(double _longitude, double _latitude)
    : longitude{_longitude}
    , latitude{_latitude}
    {
    }
    void set(bool longitude, double value) {
        if (longitude) {
            this->longitude = value;
        }
        else {
            this->latitude = value;
        }
    }
    void min(const GeoCoord& min) {
        longitude = std::min(min.longitude, longitude);
        latitude = std::min(min.latitude, latitude);
    }
    void max(const GeoCoord& max) {
        longitude = std::max(max.longitude, longitude);
        latitude = std::max(max.latitude, latitude);
    }
    auto floor() const ->GeoCoord {
        return GeoCoord{std::floor(longitude), std::floor(latitude)};
    }
    auto ceil() const ->GeoCoord {
        return GeoCoord{std::ceil(longitude), std::ceil(latitude)};
    }
    auto operator-(const GeoCoord& sub) const->GeoCoord {
        return GeoCoord{longitude - sub.longitude, latitude - sub.latitude};
    }
    auto operator+(const GeoCoord& add) const->GeoCoord {
        return GeoCoord{longitude + add.longitude, latitude + add.latitude};
    }
    [[nodiscard]] auto getLongitude() const
    {
        return longitude;
    }
    [[nodiscard]] auto getLatitude() const
    {
        return latitude;
    }
protected:
    double longitude;
    double latitude;
};

class GeoConversion {
public:
    GeoConversion() = default;
    explicit GeoConversion(const GeoConversion& other) = delete;
    virtual ~GeoConversion() = default;

     virtual GeoCoord toDisplay(const GeoCoord& geoCoord) = 0;
     virtual GeoCoord fromDisplay(const GeoCoord& geoCoord) = 0;
};

class GeoConvLinear
: public GeoConversion
{
public:
    GeoConvLinear() = default;
    explicit GeoConvLinear(const GeoConvLinear& other) = delete;
    virtual ~GeoConvLinear() = default;

     GeoCoord toDisplay(const GeoCoord& geoCoord) override;
     GeoCoord fromDisplay(const GeoCoord& geoCoord) override;
};
