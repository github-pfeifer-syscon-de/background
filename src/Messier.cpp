/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf
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



#include "Messier.hpp"

Messier::Messier()
{
}

std::shared_ptr<RaDec>
Messier::getRaDec()
{
	return raDec;
}

void
Messier::setRaDec(const std::shared_ptr<RaDec>& _raDec)
{
	raDec = _raDec;
}

double
Messier::getVmagnitude() const
{
	return m_vmagnitude;
}

void
Messier::setVmagnitude(double _vmagnitude)
{
	m_vmagnitude = _vmagnitude;
}

void
Messier::setName(const Glib::ustring& _ident)
{
	m_name = _ident;
}

Glib::ustring 
Messier::getName()
{
    return m_name;
}