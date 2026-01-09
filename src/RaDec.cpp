/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2023 RPf
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


#include "RaDec.hpp"

#include "Math.hpp"

RaDec::RaDec()
{
}


RaDec::RaDec(double _ra, double _dec)
: ra{_ra}
, dec{_dec}
{
}

void
RaDec::setRaRad(double _ra)
{
    ra = _ra;
}

double
RaDec::getRaRad() const
{
    return ra;
}

void
RaDec::setDecRad(double _dec)
{
    dec = _dec;
}

double
RaDec::getDecRad() const
{
    return dec;
}

void
RaDec::setRaDegrees(double raDeg)
{
    ra = Math::toRadians(raDeg);
}

double
RaDec::getRaDegrees()
{
return Math::toDegrees(ra);
}

void
RaDec::setRaHours(double raHours)
{
    ra = Math::toRadianHours(raHours);
}

double
RaDec::getRaHours()
{
    return Math::toHoursRadian(ra);
}

void
RaDec::setDecDegrees(double decDeg)
{
    dec = Math::toRadians(decDeg);
}

void
RaDec::setDecDegreesPolar(double decDegPolar)
{
    setDecDegrees(90.0 - decDegPolar);
}

double
RaDec::getDecDegrees()
{
    return Math::toDegrees(dec);
}

bool
RaDec::operator==(const RaDec& rhs)
{
    return dec == rhs.dec && ra == rhs.ra;
}

