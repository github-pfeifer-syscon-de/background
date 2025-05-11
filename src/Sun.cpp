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

#include <cmath>

#include "Sun.hpp"
#include "Math.hpp"

Sun::Sun()
{
}

std::shared_ptr<RaDec>
Sun::position(const JulianDate& jd)
{
	double n = jd.toJulianDateE2000day();
	double L = std::fmod((280.460 + 0.9856474 * n), 360.0);
	double g = Math::toRadians(std::fmod((357.528 + 0.9856003 * n), 360.0));
	if (L < 0.0) {
	    L += 360.0;
	}
	if (g < 0.0) {
	    g += Math::TWO_PI;
	}
	double lambda = Math::toRadians(L+1.915*std::sin(g)+0.020*std::sin(2.0*g));
	//double beta = 0.0;
	double eps = Math::toRadians(23.439 - 0.0000004 * n);
	double ra = std::atan2(std::cos(eps) * std::sin(lambda), std::cos(lambda));
	double dec= std::asin(std::sin(eps)*std::sin(lambda));
	if (ra < 0.0) {
	    ra += Math::TWO_PI;
	}
	return std::make_shared<RaDec>(ra, dec);
}

