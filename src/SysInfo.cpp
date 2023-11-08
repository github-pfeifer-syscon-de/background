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
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <fstream>              // ifstream
#include <sys/utsname.h>        // uname
#include <bitset>
#include <gtkmm.h>

#include "SysInfo.hpp"

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
    struct utsname utsname;
    int ret = uname(&utsname);
    if (ret == 0) {
        return utsname.nodename;
    }
    return "";
}

std::string
SysInfo::machine()
{
    struct utsname utsname;
    int ret = uname(&utsname);
    if (ret == 0) {
        return utsname.machine;
    }
    return "";
    //ctx->show_text(utsname.domainname);  // (none)
}

std::string
SysInfo::osVersion()
{
    struct utsname utsname;
    int ret = uname(&utsname);
    if (ret == 0) {
        return Glib::ustring::sprintf("%s %s", utsname.sysname, utsname.release);
    }
    return "";
}


std::string
SysInfo::netConn(const std::string& netintf)
{
    std::string netInfo;
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
            if (family == AF_INET ) {   // for now use ipv4|| family == AF_INET6
                auto sa = reinterpret_cast<struct sockaddr_in *>(ifa->ifa_netmask);
                std::bitset<32>bs{sa->sin_addr.s_addr};     // this is ipv4 specific
                char host[NI_MAXHOST];
                int32_t addrSize = (family == AF_INET)
                        ? sizeof(struct sockaddr_in)
                        : sizeof(struct sockaddr_in6);
                int s = getnameinfo(ifa->ifa_addr,
                                    addrSize,
                                    host, NI_MAXHOST,
                                    NULL, 0, NI_NUMERICHOST);
                if (s == 0) {
                    netInfo += Glib::ustring::sprintf("%s/%d ", host, bs.count());
                }
                else {
                    printf("getnameinfo() failed: %s\n", gai_strerror(s));
                }
            }
            else if (family == AF_PACKET && ifa->ifa_data != NULL) {
                //struct rtnl_link_stats *stats = static_cast<struct rtnl_link_stats *>(ifa->ifa_data);
                //printf("\t\ttx_packets = %10u; rx_packets = %10u\n"
                //      "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n",
                //      stats->tx_packets, stats->rx_packets,
                //      stats->tx_bytes, stats->rx_bytes);
            }
        }
    }
    freeifaddrs(ifaddr);
    return netInfo;
}


// nice if connections infos are of interest
//
//    std::string ip;
//    int conn = 0;
//    int line = 0;
//    std::ifstream stat;
//    std::ios_base::iostate exceptionMask = stat.exceptions() | std::ios::failbit | std::ios::badbit | std::ios::eofbit;
//    stat.exceptions(exceptionMask);
//    try {
//        // without any connection we wont get the info :(
//        stat.open("/proc/net/tcp");
//        while (!stat.eof()) {
//            std::string buf;
//            std::getline(stat, buf);
//            if (buf.starts_with("   ")) {   // these lines we are looking for
//                auto addr = buf.find(' ', 4);
//                if (ip.empty()
//                 && addr != buf.npos) {
//                    ++addr;
//                    auto end = buf.find(':', addr);
//                    if (end  != buf.npos) {
//                        auto hexip = buf.substr(addr, end - addr);
//                        if (hexip.length() == 8) {
//                            //std::cout << "Handling hexip " << hexip << std::endl;
//                            auto h2 = std::stoi(hexip.substr(6, 2), nullptr, 16);
//                            auto h1 = std::stoi(hexip.substr(4, 2), nullptr, 16);
//                            auto l2 = std::stoi(hexip.substr(2, 2), nullptr, 16);
//                            auto l1 = std::stoi(hexip.substr(0, 2), nullptr, 16);
//                            ip = Glib::ustring::sprintf("%d.%d.%d.%d", h2, h1, l2, l1);
//                        }
//                        else {
//                            std::cout << "StarDraw::netConn unexpected len " << hexip.length() << std::endl;
//                            break;
//                        }
//                    }
//                    else {
//                        std::cout << "StarDraw::netConn delimiter ':' not found from " << addr << std::endl;
//                        break;
//                    }
//                }
//                ++conn;
//            }
//            ++line;
//        }
//    }
//    catch (const std::ios_base::failure& e) {
//        if (errno != EAGAIN) {  //we get no eof as this no "normal" file...
//            std::ostringstream oss1;
//            oss1 << "Could not open /proc/net/tcp " << errno
//                 << " " << strerror(errno)
//                 << " line " << line
//                 << " ecode " << e.code();
//            // e.code() == std::io_errc::stream doesn't help either
//            ip = oss1.str();
//        }
//    }
//    if (stat.is_open()) {
//        stat.close();
//    }
//    return ip; //Glib::ustring::sprintf("%s conn %d", ip, conn);


std::string
SysInfo::cpuInfo()
{
	std::string buf;
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
}


std::string
SysInfo::memInfo()
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
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
#pragma GCC diagnostic pop
}

std::string
SysInfo::netInfo()
{
	DIR *dir;
	struct dirent *ent;
	const char *sdir = "/sys/class/net";
	if ((dir = opendir(sdir)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_LNK
				&& strncmp(ent->d_name, "e", 1) == 0) {
				auto path = Glib::ustring::sprintf("%s/%s/speed", sdir, ent->d_name);
				unsigned int speed = std::stoi(cat(path));
				char unit = 'M';
				if (speed >= 1000) {
					speed /= 1000;
					unit = 'G';
				}

				path = Glib::ustring::sprintf("%s/%s/duplex", sdir, ent->d_name);
				auto duplex = cat(path);

				path = Glib::ustring::sprintf("%s/%s/operstate", sdir, ent->d_name);
				auto updown = cat(path);

                auto conn = netConn(ent->d_name);

				std::ostringstream oss1;
				oss1 << ent->d_name
					 << " " << speed << unit
					 << " " << duplex
					 << " " << updown
					 << " " << conn;
				closedir(dir);
				return oss1.str();		// show only first adapter
			}
		}
		closedir(dir);
	}
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