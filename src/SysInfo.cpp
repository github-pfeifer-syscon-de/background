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

#include "FileLoader.hpp"
#include "SysInfo.hpp"

// for windows the infos are sparse
SysInfo::SysInfo()
{
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

// or maybe use https://stackoverflow.com/questions/152016/detecting-cpu-architecture-compile-time
extern "C" {
    const char *getBuild() { //Get current architecture, detectx nearly every architecture. Coded by Freak
        #if defined(__x86_64__) || defined(_M_X64)
        return "x86_64";
        #elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
        return "x86_32";
        #elif defined(__ARM_ARCH_2__)
        return "ARM2";
        #elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
        return "ARM3";
        #elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
        return "ARM4T";
        #elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
        return "ARM5"
        #elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
        return "ARM6T2";
        #elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
        return "ARM6";
        #elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        return "ARM7";
        #elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        return "ARM7A";
        #elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        return "ARM7R";
        #elif defined(__ARM_ARCH_7M__)
        return "ARM7M";
        #elif defined(__ARM_ARCH_7S__)
        return "ARM7S";
        #elif defined(__aarch64__) || defined(_M_ARM64)
        return "ARM64";
        #elif defined(mips) || defined(__mips__) || defined(__mips)
        return "MIPS";
        #elif defined(__sh__)
        return "SUPERH";
        #elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
        return "POWERPC";
        #elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
        return "POWERPC64";
        #elif defined(__sparc__) || defined(__sparc)
        return "SPARC";
        #elif defined(__m68k__)
        return "M68K";
        #else
        return "UNKNOWN";
        #endif
    }
}

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
    // see https://www.felixcloutier.com/x86/cpuid
    unsigned int CPUInfo[4] {};
    char CPUBrandString[0x44] {};
    //   http://www.felixcloutier.com/x86/CPUID.html
    //   https://www.microbe.cz/docs/CPUID.pdf
    __cpuid(0x00000000, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);    // that sould be generic
//    unsigned int nStd = CPUInfo[0];
    memcpy(CPUBrandString, &CPUInfo[1], sizeof(CPUInfo[1]));
    memcpy(CPUBrandString+4, &CPUInfo[3], sizeof(CPUInfo[3]));
    memcpy(CPUBrandString+8, &CPUInfo[2], sizeof(CPUInfo[2]));
    //std::cout << "build " << getBuild() << std::endl;
    if (strcmp(CPUBrandString, "GenuineIntel") == 0) {   // unsure the following will work for non intel?
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
        memset(CPUBrandString, 0, sizeof(CPUBrandString));
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
        // std::cout << "Brand " << CPUBrandString << std::endl;
    }
    return std::string(CPUBrandString);
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
                LineReader lineReader(Gio::File::create_for_path(path));
                std::string updown;
                if (lineReader.hasNext()) {
                    lineReader.next(updown);
                }
                if (updown == "up") {
                    path = Glib::ustring::sprintf("%s/%s/speed", sdir, ent->d_name);
                    LineReader lineReader2(Gio::File::create_for_path(path));
                    unsigned int speed{};
                    char unit = 'M';
                    if (lineReader2.hasNext()) {
                        std::string speedd;
                        lineReader2.next(speedd);
                        speed = std::stoi(speedd);
                        if (speed >= 1000) {
                            speed /= 1000;
                            unit = 'G';
                        }
                    }
                    path = Glib::ustring::sprintf("%s/%s/duplex", sdir, ent->d_name);
                    LineReader lineReader3(Gio::File::create_for_path(path));
                    std::string duplex;
                    if (lineReader3.hasNext()) {
                        lineReader3.next(duplex);
                    }
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