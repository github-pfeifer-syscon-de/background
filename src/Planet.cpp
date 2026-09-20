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

#include <cmath>
#include <iostream>

#include "Planet.hpp"
#include "Planets.hpp"
#include "Math.hpp"
#include "background_config.h"

Planet::Planet(const std::string& name
              , const Elements& elements
              , const Elements& rates
              , const std::array<double,4>& extraTerms)
: m_name{name}
, m_elements{elements}
, m_rates{rates}
, m_extraTerms{extraTerms}
{
}

// will give value relative to sun
CarthesianCoord
Planet::computePlanetPosition(const JulianDate& jd)
{
	//Algorithm from Explanatory Supplement to the Astronomical Almanac ch8 P340
	//Step 1:
	double T = jd.toJulianDateE2000centuries();
	double a = m_elements.a + m_rates.a * T;
	double e = m_elements.e + m_rates.e * T;
	double I = m_elements.I + m_rates.I * T;
	double L = m_elements.L + m_rates.L * T;
	double w = m_elements.w + m_rates.w * T;
	double O = m_elements.O + m_rates.O * T;

	//Step 2:
	double ww = w - O;
	double M = L - w;
	if (m_extraTerms[0] != 0.0
     || m_extraTerms[1] != 0.0
     || m_extraTerms[2] != 0.0
     || m_extraTerms[3] != 0.0) {	// we stick to the short method so these will not be used
	    double b = m_extraTerms[0];
	    double c = m_extraTerms[1];
	    double s = m_extraTerms[2];
	    double f = m_extraTerms[3];
	    M = L - w + b*T*T + c*std::cos(Math::toRadians(f*T)) + s*std::sin(Math::toRadians(f*T));
	}

	while (M > 180.0) {
	    M -= 360.0;
	}

	double E = M + 57.29578 * e * std::sin(Math::toRadians(M));
	double dE = 1.0;
	int n = 0;
	while (std::abs(dE) > 1e-7 && n < 10) {
	    dE = solveKepler(M,e,E);
	    E += dE;
	    n++;
	}
	//System.out.format("%s E %.8f n %d\n", getName(), dE, n);

	//Step 4:
	double xp=a*(std::cos(Math::toRadians(E))-e);
	double yp=a*std::sqrt(1-e*e)*std::sin(Math::toRadians(E));
	//double zp=0;

	//Step 5:
	a = Math::toRadians(a);
	e = Math::toRadians(e);
	I = Math::toRadians(I);
	L = Math::toRadians(L);
	ww = Math::toRadians(ww);
	O = Math::toRadians(O);
	double xecl = (std::cos(ww) * std::cos(O) - std::sin(ww) * std::sin(O) * std::cos(I)) * xp + (-std::sin(ww) * std::cos(O) - std::cos(ww) * std::sin(O) * std::cos(I)) * yp;
	double yecl = (std::cos(ww) * std::sin(O) + std::sin(ww) * std::cos(O) * std::cos(I)) * xp + (-std::sin(ww) * std::sin(O) + std::cos(ww) * std::cos(O) * std::cos(I)) * yp;
	double zecl = (std::sin(ww) * std::sin(I)) * xp + (std::cos(ww) * std::sin(I)) * yp;

	//Step 6:
	const double eps = Math::toRadians(23.43928);
        const double sinEps{std::sin(eps)};
        const double cosEps{std::cos(eps)};
	return CarthesianCoord {
	    .x = xecl,
	    .y = cosEps * yecl - sinEps * zecl,
	    .z = sinEps * yecl + cosEps * zecl,};
}

double
Planet::solveKepler(double M, double e, double E)
{
	double dM = M - (E - Math::toDegrees(e) * std::sin(Math::toRadians(E)));
	double dE = dM / (1.0 - e * std::cos(Math::toRadians(E)));
	return dE;
}



CarthesianCoord
Planet::posToEarth(const JulianDate& jd)
{
	auto xyzPlanet = computePlanetPosition(jd);
    //std::cout << getName() << xyzPlanet[0] << "," << xyzPlanet[1] << "," << xyzPlanet[2] << std::endl;
    Planets planets;
    auto earth = planets.getEarth();
    //Earth earth;
    auto xyzEarth = earth->computePlanetPosition(jd);
    //std::cout << "earth " << xyzEarth[0] << "," << xyzEarth[1] << "," << xyzEarth[2] << std::endl;
    auto xyz = xyzPlanet - xyzEarth;
    //std::cout << getName() << " xyz " << xyz.x << "," << xyz.y << "," << xyz.z << std::endl;
    return xyz;
}

std::shared_ptr<RaDecPlanet>
Planet::getRaDecPositon(const JulianDate& jd)
{
    CarthesianCoord xyzRel = posToEarth(jd);
    return rectToPolar(xyzRel);
}

std::shared_ptr<RaDecPlanet>
Planet::rectToPolar(const CarthesianCoord& xyz)
{
    return xyz.toRaDecPlanet();
}

std::string
Planet::getName()
{
    return m_name;
}

const Elements&
Planet::getElements()
{
    return m_elements;
}

const Elements&
Planet::getRates()
{
    return m_rates;
}

std::array<double,4>
Planet::getExtraTerms()
{
	return m_extraTerms;
}
