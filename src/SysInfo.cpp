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

#include "SysInfo.hpp"
#ifdef __WIN32__
#include "SysInfoWindows.hpp"
#else
#include "SysInfoLinux.hpp"
#endif

SysInfo::SysInfo()
{
}

std::unique_ptr<SysInfo> 
SysInfo::create()
{
#   ifdef __WIN32__    
    return std::make_unique<SysInfoWindows>();
#   else
    return std::make_unique<SysInfoLinux>();
#   endif    
}


std::list<std::string>
SysInfo::allInfos()
{
    std::list<std::string> infos;
    infos.push_back(nodeName());
    infos.push_back(machine());
    infos.push_back(osVersion());
    infos.push_back(cpuInfo());
    infos.push_back(memInfo());
    infos.push_back(netInfo());

    return infos;
}
