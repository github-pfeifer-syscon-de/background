/*
 * Copyright (C) 2024 RPf <gpl3@pfeifer-syscon.de>
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
#include <memory>
#include <cstdlib>
#include <cmath>

#include "JulianDate.hpp"
#include "GeoPosition.hpp"
#include "RaDec.hpp"
#include "Math.hpp"
#include "Venus.hpp"

static constexpr auto expAz = 155.96;
static constexpr auto expAlt = 69.0103;
static constexpr auto raExp = 2.09998;
static constexpr auto decExp = 11.4938;
static constexpr auto distExp = 1.70358;

static bool
test_azAlt()
{
    JulianDate jd{2459349.210248739};
    GeoPosition geoPos{274.236400, 38.2464000};
    auto raDec = std::make_shared<RaDec>();
    raDec->setRaHours(3.4299545354);
    raDec->setDecDegrees(18.726708585);
    auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
    std::cout << "is  az " << azAlt->getAzimutDegrees() << " alt " << azAlt->getAltitudeDegrees() << std::endl;
    std::cout << "exp az " << expAz << " alt " << expAlt << std::endl;
    if (std::abs(azAlt->getAzimutDegrees()  - expAz) > 0.1 ||
        std::abs(azAlt->getAltitudeDegrees()  - expAlt) > 0.1) {
        return false;
    }
    return true;
}


static bool
test_deg2rad()
{
    auto rad = Math::toRadians(10.0);
    std::cout << "   deg rad " << rad << std::endl;
    if (std::abs(rad - 0.174533) > 0.00001) {
        return false;
    }
    auto deg = Math::toDegrees(rad);
    std::cout << "is  deg " << deg << std::endl;
    std::cout << "exp deg " << 10.0 << std::endl;
    if (std::abs(deg - 10.0) > 0.001) {
        return false;
    }
    return true;
}

static bool
test_hour2rad()
{
    auto rad = Math::toRadianHours(10.0);
    std::cout << "   hour rad " << rad << std::endl;
    if (std::abs(rad - 2.61799) > 0.00001) {
        return false;
    }
    auto hour = Math::toHoursRadian(rad);
    std::cout << "is  hour " << hour << std::endl;
    std::cout << "exp hour " << 10.0 << std::endl;
    if (std::abs(hour - 10.0) > 0.001) {
        return false;
    }
    return true;
}

static bool
test_planet()
{
    // see https://celestialprogramming.com/planets_with_keplers_equation.html
    Glib::DateTime date = Glib::DateTime::create_utc(2024, 5, 2, 10, 21, 0);
    JulianDate jd{date};
    Venus venus;
    auto raDec = venus.getRaDecPositon(jd);
    std::cout << "Venus ra " << raDec->getRaHours()
              << " dec " << raDec->getDecDegrees()
              << " dist " << raDec->getDistanceAU() << std::endl;
    if (std::abs(raDec->getRaHours() - raExp) > 0.001 ||
        std::abs(raDec->getDecDegrees() - decExp) > 0.001 ||
        std::abs(raDec->getDistanceAU() - distExp) > 0.1) {
        return false;
    }
    return true;
}
/*
 *
 */
int main(int argc, char** argv)
{
    if (!test_azAlt()) {
        return 1;
    }
    if (!test_deg2rad()) {
        return 2;
    }
    if (!test_hour2rad()) {
        return 3;
    }
    if (!test_planet()) {
        return 4;
    }
    return 0;
}

