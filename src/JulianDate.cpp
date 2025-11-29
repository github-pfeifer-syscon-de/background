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

#include <iostream>

#include "JulianDate.hpp"

JulianDate::JulianDate(const Glib::DateTime&  cal)
: JulianDate(((double)cal.to_unix() / SEC_PER_JULIAN_DAY) + JULIAN_1970_OFFS)
{
}

JulianDate::JulianDate(double _jd)
{
	jd = _jd;
}

double
JulianDate::getJulianDate() const
{
	return jd;
}

// to epoch
double
JulianDate::toJulianDateE2000centuries() const
{
	return toJulianDateE2000day() / DAYS_PER_CENTURY;
}

// to epoch
double
JulianDate::toJulianDateE2000day() const
{
	return jd - E2000_JULIAN;
}

