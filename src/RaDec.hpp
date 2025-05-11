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

/**
 * Right Ascension, Declination
 *
 * @author RPf <gpl3@pfeifer-syscon.de>
 */
class RaDec
{
public:
    RaDec();
    RaDec(double ra, double dec);
    RaDec(const RaDec& orig) = default;
    virtual ~RaDec() = default;

    void setRaRad(double ra);
    double getRaRad() const;
    void setDecRad(double dec);
    double getDecRad() const;
    void setRaDegrees(double raDeg);
    double getRaDegrees();
    void setRaHours(double raHours);
    double getRaHours();
    void setDecDegrees(double decDeg);
    void setDecDegreesPolar(double decDegPolar);
    double getDecDegrees();

    bool operator==(const RaDec& rhs);

private:
    double ra{}; // values in radians
    double dec{};

};
