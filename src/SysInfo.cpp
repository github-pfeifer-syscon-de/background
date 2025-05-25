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

#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef __linux
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <sys/utsname.h>        // uname
#endif
#ifdef  __WIN32__
#include <Windows.h>
#include <tchar.h>
#include <cpuid.h>
#endif
#include <fstream>              // ifstream
#include <bitset>
#include <gtkmm.h>

#include "SysInfo.hpp"

// for windows the infos are sparse
SysInfo::SysInfo()
{
}

static std::string
cat(const std::string& path)
{
    std::ifstream stat;
    std::ios_base::iostate exceptionMask = stat.exceptions() | std::ios::failbit | std::ios::badbit;
    stat.exceptions(exceptionMask);
    std::string dest;
    try {
        stat.open(path);
        if (!stat.eof()) {
            std::getline(stat, dest);
        }
    }
    catch (std::ios_base::failure& e) {
        dest = Glib::ustring::sprintf("err %s", path);
    }
    if (stat.is_open()) {
	    stat.close();
    }
	return dest;
}

std::string
SysInfo::nodeName()
{
#   ifdef __linux
    struct utsname utsname;
    int ret = uname(&utsname);
    if (ret == 0) {
        return utsname.nodename;
    }
#   endif
#   ifdef  __WIN32__
    //std::cout << "Hostname" << gethostname(buf, sizeof(buf)) << std::endl;
    TCHAR infoBuf[150];
    DWORD bufCharCount = sizeof(infoBuf);
    memset(infoBuf, 0, bufCharCount*sizeof(TCHAR));
    if (GetComputerName(infoBuf, &bufCharCount)) {   // this is a simple version if you feel funny use the ...W function
        Glib::ustring name;
        for (DWORD i = 0; i < bufCharCount; ++i) {
            name += static_cast<char>((infoBuf[i] & 0x7f));  // this will garble non ascii
        }
        return name;
    }
    else {
        std::cout << "SysInfo::nodeName no name " << std::endl;
    }
#   endif
    return "";
}

#ifdef  __WIN32__
static std::string
get_as_ASCII(unsigned int value)
{
    std::string ret;
    for (int i = 0; i < 4; i++) {
        char byte = static_cast<char>((value >> (i * 8)) & 0xff);
        ret += byte;
    }
    return ret;
}
#endif

std::string
SysInfo::machine()
{
#   ifdef __linux
    struct utsname utsname;
    int ret = uname(&utsname);
    if (ret == 0) {
        return utsname.machine;
    }
#   endif
#   ifdef  __WIN32__
#if defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)
    char CPUBrandString[0x40];
    unsigned int CPUInfo[4] = {0,0,0,0};
    memset(CPUBrandString, 0, sizeof(CPUBrandString));
    // 0x16 not working for amd as eax is 1 for call 0x0!
    //   http://www.felixcloutier.com/x86/CPUID.html
    //   https://www.microbe.cz/docs/CPUID.pdf
//    __cpuid(0x00000000, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
//    unsigned int nStd = CPUInfo[0];
//    memcpy(CPUBrandString, &CPUInfo[1], sizeof(CPUInfo[1]));
//    memcpy(CPUBrandString+4, &CPUInfo[3], sizeof(CPUInfo[3]));
//    memcpy(CPUBrandString+8, &CPUInfo[2], sizeof(CPUInfo[2]));
//    std::cout << "std " << nStd << " id " << CPUBrandString << std::endl;

//    CPUInfo[0] = 0;
//    CPUInfo[1] = 0;
//    CPUInfo[2] = 0;
//    CPUInfo[3] = 0;
//    __cpuid(0x00000001, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
//    std::cout << "eax 0x" << std::hex << CPUInfo[0] << std::endl;
//    std::cout << "ebx 0x" << std::hex << CPUInfo[1] << std::endl;
//    std::cout << "ecx 0x" << std::hex << CPUInfo[2] << std::endl;
//    std::cout << "edx 0x" << std::hex << CPUInfo[3] << std::endl;

    CPUInfo[0] = 0;
    CPUInfo[1] = 0;
    CPUInfo[2] = 0;
    CPUInfo[3] = 0;
    __cpuid(0x80000000, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
    unsigned int nExIds = CPUInfo[0];
    memset(CPUBrandString, 0, sizeof(CPUBrandString));
    for (unsigned int i = 0x80000002; i <= nExIds; ++i) {
        __cpuid(i, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
        if (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }
    std::cout << "Brand " << CPUBrandString << std::endl;

    return std::string(CPUBrandString);
#   else
    // as the windows GetSystemInfo is outdated, and encoded ...
    unsigned int eax, ebx, ecx, edx;
    // EAX=0: Highest Function Parameter and Manufacturer ID
    unsigned int ret = __get_cpuid(0, &eax, &ebx, &ecx, &edx);
    if (ret == 1) {
        std::string cpuid = get_as_ASCII(ebx) + get_as_ASCII(edx) +  get_as_ASCII(ecx);
        return cpuid;
    }
    else {
        std::cout << "Get cpuid failed ret " << ret << std::endl;
    }
#   endif
    return "";
}

std::string
SysInfo::osVersion()
{
#   ifdef __linux
    struct utsname utsname;
    int ret = uname(&utsname);
    if (ret == 0) {
        return Glib::ustring::sprintf("%s %s", utsname.sysname, utsname.release);
    }
#   endif
    return "";
}


std::string
SysInfo::netConn(const std::string& netintf)
{
    std::string netInfo;
#   ifdef __linux
    struct ifaddrs *ifaddr;

    if (getifaddrs(&ifaddr) == -1) {
        return "";
    }

    /* Walk through linked list, maintaining head pointer so we
       can free list later. */
    for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        int family = ifa->ifa_addr->sa_family;

        /* Display interface name and family (including symbolic
           form of the latter for the common families). */
        //intf += Glib::ustring::sprintf("%-8s %s (%d)\n",
        //        ifa->ifa_name,
        //        (family == AF_PACKET) ? "AF_PACKET"
        //        : (family == AF_INET) ? "AF_INET"
        //        : (family == AF_INET6) ? "AF_INET6"
        //        : "???", family);

        /* For an AF_INET* interface address, display the address. */
        if (strcmp(ifa->ifa_name, netintf.c_str()) == 0) {
            if (family == AF_INET) {
                auto sa = reinterpret_cast<struct sockaddr_in *>(ifa->ifa_netmask);
                std::bitset<32>bs{sa->sin_addr.s_addr};     // this is ipv4 specific
                char host[NI_MAXHOST];
                int s = getnameinfo(ifa->ifa_addr,
                                    sizeof(struct sockaddr_in),
                                    host, NI_MAXHOST,
                                    NULL, 0, NI_NUMERICHOST);
                if (s == 0) {
                    netInfo += Glib::ustring::sprintf("%s/%d ", host, bs.count());
                }
                else {
                    printf("getnameinfo() failed: %s\n", gai_strerror(s));
                }
            }
            // not that informative & issue getting netmask
            //else if (family == AF_INET6) {
            //    auto sa = reinterpret_cast<struct sockaddr_in6 *>(ifa->ifa_netmask);
            //    std::bitset<128>bs{sa->sin6_addr.s6_addr};
            //    char host[NI_MAXHOST];
            //    int s = getnameinfo(ifa->ifa_addr,
            //                        sizeof(struct sockaddr_in6),
            //                        host, NI_MAXHOST,
            //                        NULL, 0, NI_NUMERICHOST);
            //    if (s == 0) {
            //        netInfo += Glib::ustring::sprintf("%s/%d ", host, bs.count());
            //    }
            //    else {
            //        printf("getnameinfo() failed: %s\n", gai_strerror(s));
            //    }
            //}
            //else if (family == AF_PACKET && ifa->ifa_data != NULL) {
            //    struct rtnl_link_stats *stats = static_cast<struct rtnl_link_stats *>(ifa->ifa_data);
            //    printf("\t\ttx_packets = %10u; rx_packets = %10u\n"
            //          "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n",
            //          stats->tx_packets, stats->rx_packets,
            //          stats->tx_bytes, stats->rx_bytes);
            //}
        }
    }
    freeifaddrs(ifaddr);
#   endif
    return netInfo;
}


std::string
SysInfo::cpuInfo()
{
	std::string buf;
#   ifdef __linux
	unsigned int readmask = 0;
	const unsigned int ALL_MASK = 0x01;

	std::ifstream  stat;
	std::ios_base::iostate exceptionMask = stat.exceptions() | std::ios::failbit | std::ios::badbit;
	stat.exceptions(exceptionMask);
	try {
	    stat.open("/proc/cpuinfo");
	    while (readmask != ALL_MASK && !stat.eof()) {
	        std::getline(stat, buf);
	        if (buf.starts_with("model name")) {
	            readmask |= 0x01;
				break;
	        }
	    }
	}
	catch (std::ios_base::failure& e) {
	    std::ostringstream oss1;
		oss1 << "Could not open /proc/cpuinfo: " << errno << " " << strerror(errno);
	    return oss1.str();
	}
	if (readmask != ALL_MASK) {
	    std::ostringstream oss1;
		oss1 << "Couldn't read all values from /proc/cpuinfo: readmask " << readmask;
	    return oss1.str();
	}
    if (stat.is_open()) {
	    stat.close();
    }
	auto value = buf.find(':');
	if (value == buf.npos) {
		return buf;
	}
	else {
		value += 2;
	}
	return std::string(buf.substr(value));
#   endif
    return "";
}


std::string
SysInfo::memInfo()
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#   ifdef __linux
    unsigned int readmask = 0;
    const unsigned int ALL_MASK = 0x0f;
    unsigned long mem_total = 0l,mem_avail = 0l,mem_buffers = 0l,mem_cached = 0l;
    std::ifstream  stat;
    std::ios_base::iostate exceptionMask = stat.exceptions() | std::ios::failbit | std::ios::badbit;
    stat.exceptions(exceptionMask);

    try {
        stat.open("/proc/meminfo");
        while (readmask != ALL_MASK && !stat.eof()) {
            std::string buf;
            std::getline(stat, buf);
            auto pos = buf.find(' ');
            if (buf.starts_with("MemTotal:") && pos != buf.npos) {
                mem_total = std::stoi(buf.substr(pos));
                readmask |= 0x01;
            }
            else if (buf.starts_with("MemAvailable:") && pos != buf.npos) {
                mem_avail = std::stoi(buf.substr(pos));
                readmask |= 0x02;
            }
            else if (buf.starts_with("Buffers:") && pos != buf.npos) {
                mem_buffers = std::stoi(buf.substr(pos));
                readmask |= 0x04;
            }
            else if (buf.starts_with("Cached:")  && pos != buf.npos) {
                mem_cached =  std::stoi(buf.substr(pos));
                readmask |= 0x08;
            }
        }
	}
	catch (std::ios_base::failure& e) {
	    std::ostringstream oss1;
	    oss1 << "Could not open /proc/meminfo: " << errno  << " " << strerror(errno);
	    return oss1.str();
	}
	if (readmask != ALL_MASK) {
	    std::ostringstream oss1;
	    oss1 << "Couldn't read all values from /proc/meminfo: readmask " <<  readmask;
	    return oss1.str();
	}
    if (stat.is_open()) {
        stat.close();
    }
	std::ostringstream oss1;
	oss1 << (mem_total - mem_avail) / 1024 << "MB used of " <<  mem_total / 1024 << "MB";
	return oss1.str();
#   endif
        return "";
#pragma GCC diagnostic pop
}

std::string
SysInfo::netInfo()
{
#   ifdef __linux
	DIR *dir;
	struct dirent *ent;
	const char *sdir = "/sys/class/net";
	if ((dir = opendir(sdir)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_LNK
				&& strncmp(ent->d_name, "e", 1) == 0) {
   				std::ostringstream oss1;
				auto path = Glib::ustring::sprintf("%s/%s/operstate", sdir, ent->d_name);
				auto updown = cat(path);
                if (updown == "up") {
                    path = Glib::ustring::sprintf("%s/%s/speed", sdir, ent->d_name);
                    unsigned int speed = std::stoi(cat(path));
                    char unit = 'M';
                    if (speed >= 1000) {
                        speed /= 1000;
                        unit = 'G';
                    }

                    path = Glib::ustring::sprintf("%s/%s/duplex", sdir, ent->d_name);
                    auto duplex = cat(path);

                    auto conn = netConn(ent->d_name);
                    oss1 << ent->d_name
                         << " " << speed << unit
                         << " " << duplex
                         << " " << updown
                         << " " << conn;
                }
                else {      // not much infos for this case
                    oss1 << ent->d_name
                         << " " << updown;
                }
				closedir(dir);
				return oss1.str();		// show only first adapter
			}
		}
		closedir(dir);
	}
#   endif
    return std::string("No adapter found");
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