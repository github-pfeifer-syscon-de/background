/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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
#include <fstream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <Windows.h>
#include <cpuid.h>
#include <gtkmm.h>

#include "SysInfoWindows.hpp"

SysInfoWindows::SysInfoWindows() 
{
}

std::string
SysInfoWindows::nodeName()
{
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

    return "";
}

std::string
SysInfoWindows::machine()
{
    // uses https://stackoverflow.com/questions/152016/detecting-cpu-architecture-compile-time
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


std::string
SysInfoWindows::osVersion()
{
    OSVERSIONINFOA osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOA));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    GetVersionEx(&osvi);
    const char* osName = nullptr;
    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-osversioninfoa
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        if (osvi.dwMajorVersion >= 10) {
            if (osvi.dwBuildNumber >= 22000) {
                osName = "11";
            }
            else {
                osName = "10";
            }            
        }
        else if (osvi.dwMajorVersion >= 6) {
            if (osvi.dwMinorVersion >= 3) {
                osName = "8.1";
            }
            else if (osvi.dwMinorVersion >= 2) {
                osName = "8";
            }
            else if (osvi.dwMinorVersion >= 1) { 
                osName = "7";
            }
            else {
                osName = "Vista";
            }
        }
        else if (osvi.dwMajorVersion >= 5) {
            if (osvi.dwMinorVersion >= 1) {
                osName = "XP";
            }        
            else {
                osName = "2000";
            }            
        }
    }
    else {
        if (osvi.dwMajorVersion >= 10) {
            osName = "Server 2016";
        }
        else if (osvi.dwMajorVersion >= 6) {
            if (osvi.dwMinorVersion >= 3) {
                osName = "Server 2012 R2";
            }
            else if (osvi.dwMinorVersion >= 2) {
                osName = "Server 2012";
            }
            else if (osvi.dwMinorVersion >= 1) {
                osName = "Server 2008 R2";
            }
            else {
                osName = "Server 2008";
            }
        }
        else if (osvi.dwMajorVersion >= 5) {
            if (osvi.dwMinorVersion >= 2) {
                if (GetSystemMetrics(SM_SERVERR2) != 0) {
                    osName = "Server 2003 R2";
                }
                else {
                    osName = "Server 2003";
                }
            }        
            else if (osvi.dwMinorVersion >= 1) {
                osName = "XP";
            }
            else {
                osName = "2000";
            }            
        }
    }
    if (osName != nullptr) {
        Glib::ustring version;
        if (strlen(osvi.szCSDVersion) > 0) {
            version = Glib::ustring(osvi.szCSDVersion);            
        }
        else {
            version = Glib::ustring::sprintf("Build %d", osvi.dwBuildNumber);
        }
        return Glib::ustring::sprintf("Windows %s %s", osName, version);
    }
    else {
        return Glib::ustring::sprintf("Windows ? maj %d min %d bld %d ver %s",
            osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.szCSDVersion);        
    }
}

std::string
SysInfoWindows::cpuInfo()
{
#   ifdef __x86_64__
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
SysInfoWindows::memInfo()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);        
        
    std::ostringstream oss1;
    oss1 << (statex.ullTotalPhys -  statex.ullAvailPhys) / BYTE_TO_MEGA << "MB used of " <<  statex.ullTotalPhys / BYTE_TO_MEGA << "MB";
    return oss1.str();
}

#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

std::string
SysInfoWindows::netInfo()
{
    // Link with Iphlpapi.lib
#   ifdef Iphlpapi_lib
    // see https://learn.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersaddresses
    DWORD dwRetVal = 0;

    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    LPVOID lpMsgBuf = NULL;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
    IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
    IP_ADAPTER_PREFIX *pPrefix = NULL;
    
    ULONG family = AF_INET;   // look for ip4, all = AF_UNSPEC, ipv6 = AF_INET6 
    
   // Allocate a 15 KB buffer to start with.
    outBufLen = WORKING_BUFFER_SIZE;

    do {

        pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);
        if (pAddresses == NULL) {
            printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
            return "";
        }

        dwRetVal =
            GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = NULL;
        } else {
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR) {
        // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            printf("\tLength of the IP_ADAPTER_ADDRESS struct: %ld\n",
                   pCurrAddresses->Length);
            printf("\tIfIndex (IPv4 interface): %u\n", pCurrAddresses->IfIndex);
            printf("\tAdapter name: %s\n", pCurrAddresses->AdapterName);

            pUnicast = pCurrAddresses->FirstUnicastAddress;
            if (pUnicast != NULL) {
                for (i = 0; pUnicast != NULL; i++)
                    pUnicast = pUnicast->Next;
                printf("\tNumber of Unicast Addresses: %d\n", i);
            } else
                printf("\tNo Unicast Addresses\n");

            pAnycast = pCurrAddresses->FirstAnycastAddress;
            if (pAnycast) {
                for (i = 0; pAnycast != NULL; i++)
                    pAnycast = pAnycast->Next;
                printf("\tNumber of Anycast Addresses: %d\n", i);
            } else
                printf("\tNo Anycast Addresses\n");

            pMulticast = pCurrAddresses->FirstMulticastAddress;
            if (pMulticast) {
                for (i = 0; pMulticast != NULL; i++)
                    pMulticast = pMulticast->Next;
                printf("\tNumber of Multicast Addresses: %d\n", i);
            } else
                printf("\tNo Multicast Addresses\n");

            pDnServer = pCurrAddresses->FirstDnsServerAddress;
            if (pDnServer) {
                for (i = 0; pDnServer != NULL; i++)
                    pDnServer = pDnServer->Next;
                printf("\tNumber of DNS Server Addresses: %d\n", i);
            } else
                printf("\tNo DNS Server Addresses\n");

            printf("\tDNS Suffix: %wS\n", pCurrAddresses->DnsSuffix);
            printf("\tDescription: %wS\n", pCurrAddresses->Description);
            printf("\tFriendly name: %wS\n", pCurrAddresses->FriendlyName);

            if (pCurrAddresses->PhysicalAddressLength != 0) {
                printf("\tPhysical address: ");
                for (i = 0; i < (int) pCurrAddresses->PhysicalAddressLength;
                     i++) {
                    if (i == (pCurrAddresses->PhysicalAddressLength - 1))
                        printf("%.2X\n",
                               (int) pCurrAddresses->PhysicalAddress[i]);
                    else
                        printf("%.2X-",
                               (int) pCurrAddresses->PhysicalAddress[i]);
                }
            }
            printf("\tFlags: %ld\n", pCurrAddresses->Flags);
            printf("\tMtu: %lu\n", pCurrAddresses->Mtu);
            printf("\tIfType: %ld\n", pCurrAddresses->IfType);
            printf("\tOperStatus: %ld\n", pCurrAddresses->OperStatus);
            printf("\tIpv6IfIndex (IPv6 interface): %u\n",
                   pCurrAddresses->Ipv6IfIndex);
            printf("\tZoneIndices (hex): ");
            for (i = 0; i < 16; i++)
                printf("%lx ", pCurrAddresses->ZoneIndices[i]);
            printf("\n");

            printf("\tTransmit link speed: %I64u\n", pCurrAddresses->TransmitLinkSpeed);
            printf("\tReceive link speed: %I64u\n", pCurrAddresses->ReceiveLinkSpeed);

            pPrefix = pCurrAddresses->FirstPrefix;
            if (pPrefix) {
                for (i = 0; pPrefix != NULL; i++)
                    pPrefix = pPrefix->Next;
                printf("\tNumber of IP Adapter Prefix entries: %d\n", i);
            } else
                printf("\tNumber of IP Adapter Prefix entries: 0\n");

            printf("\n");

            pCurrAddresses = pCurrAddresses->Next;
        }
    } else {
        printf("Call to GetAdaptersAddresses failed with error: %d\n",
               dwRetVal);
        if (dwRetVal == ERROR_NO_DATA)
            printf("\tNo addresses were found for the requested parameters\n");
        else {

            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                    NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   
                    // Default language
                    (LPTSTR) & lpMsgBuf, 0, NULL)) {
                printf("\tError: %s", lpMsgBuf);
                LocalFree(lpMsgBuf);
                if (pAddresses)
                    FREE(pAddresses);
                return "";
            }
        }
    }
    if (pAddresses) {
        FREE(pAddresses);
    }
#   endif    
    
    return "";   
}