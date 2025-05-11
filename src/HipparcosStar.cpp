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

#include "HipparcosStar.hpp"

HipparcosStar::HipparcosStar()
{
}

long
HipparcosStar::getNumber() const
{
	return number;
}

void
HipparcosStar::setNumber(long _number)
{
	number = _number;
}

std::shared_ptr<RaDec>
HipparcosStar::getRaDec()
{
	return raDec;
}

void
HipparcosStar::setRaDec(const std::shared_ptr<RaDec>& _raDec)
{
	raDec = _raDec;
}

double
HipparcosStar::getVmagnitude() const
{
	return vmagnitude;
}

void
HipparcosStar::setVmagnitude(double _vmagnitude)
{
	vmagnitude = _vmagnitude;
}

void
HipparcosStar::setIdent(const std::string& _ident)
{
	ident = _ident;
}

void
HipparcosStar::setFlag(int _flag)
{
	flag = _flag;
}