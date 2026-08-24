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
#include <StringUtils.hpp>
#include <glibmm.h>

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
        
    const auto usedMb = static_cast<uint32_t>((statex.ullTotalPhys -  statex.ullAvailPhys) / BYTE_TO_MEGA);
    const auto totalMb = static_cast<uint32_t>(statex.ullTotalPhys / BYTE_TO_MEGA);
    const double percent = static_cast<double>(usedMb) * 100.0 / static_cast<double>(totalMb);
    return Glib::ustring::sprintf("%ldMiB used of %ldMiB is %.1lf%%", usedMb, totalMb, percent);
}

#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"    
// this is questionable ...
static std::string 
decodeAddr(const SOCKET_ADDRESS* sockAdr) 
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"    
    std::string info;
    if (sockAdr->lpSockaddr != nullptr) {
        auto start = info.length();
        // as we see a offset of the adress this seems to be a inaddr struct
        LPSOCKADDR_IN sockAddress = reinterpret_cast<LPSOCKADDR_IN>(sockAdr->lpSockaddr);
        if (sockAddress->sin_family == AF_INET) {
            info.reserve(32);
            for (int32_t i = 0; i < 4; ++i) {
                if (info.length() > start) {
                    info += '.';
                }
                info += Glib::ustring::sprintf("%u", *(reinterpret_cast<uint8_t*>(&sockAddress->sin_addr) + i));
            }
            // unsupported maybe try #define _WS2IPDEF_ #include <Mstcpip.h> #include <ip2string.h> .. 
            //RtlIpv4AddressToString(&sockAddress->sin_addr, reinterpret_cast<PSTR>(&info[0]));
        }
        else if(sockAddress->sin_family == AF_INET6) {
            info.reserve(64);
            for (int32_t i = 0; i < 8; ++i) {
                if (info.length() > start) {
                    info += ':';
                }
                info += Glib::ustring::sprintf("%04ux", *(reinterpret_cast<uint16_t*>(&sockAddress->sin_addr) + i));
            }            
            //RtlIpv6AddressToString(reinterpret_cast<const in6_addr*>(&sockAddress->sin_addr), reinterpret_cast<PSTR>(&info[0]));             
        }        
        else {
            info = Glib::ustring::sprintf("family 0x%x unknown", sockAddress->sin_family);
        }
    }
    return info;
#pragma GCC diagnostic pop    
}
#pragma GCC diagnostic pop    

std::string
SysInfoWindows::netAdapterInfo(uint64_t adapterIndex) 
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"    
    std::string info;
   // see https://learn.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersaddresses
    DWORD dwRetVal = 0;

    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    LPVOID lpMsgBuf = NULL;

    PIP_ADAPTER_ADDRESSES pAddresses{};
    ULONG outBufLen = 0;
    ULONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = nullptr;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = nullptr;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = nullptr;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = nullptr;
    IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = nullptr;
    IP_ADAPTER_PREFIX *pPrefix = nullptr;
    
    ULONG family = AF_INET;   // look for ip4, all = AF_UNSPEC, ipv6 = AF_INET6 
    
    // Allocate a 15 KB buffer to start with.
    outBufLen = WORKING_BUFFER_SIZE;
    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);
        if (pAddresses == NULL) {
            return "Memory allocation failed for IP_ADAPTER_ADDRESSES struct";
        }
        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = NULL;
        } 
        else {
            break;
        }
        Iterations++;
    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR) {
        // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            if (adapterIndex == pCurrAddresses->IfIndex) {
                //info += Glib::ustring::sprintf("\tLength of the IP_ADAPTER_ADDRESS struct: %ld\n", pCurrAddresses->Length);
                //info += Glib::ustring::sprintf("\tIfIndex (IPv4 interface): %lu\n", pCurrAddresses->IfIndex);
                //info += Glib::ustring::sprintf("\tAdapter name: %s\n", pCurrAddresses->AdapterName);
                //pUnicast = pCurrAddresses->FirstUnicastAddress;
                info.reserve(64);
                //if (pUnicast != NULL) {
                //    for (i = 0; pUnicast != nullptr; ++i) {
                //        info += decodeAddr(&pUnicast->Address); 
                //        pUnicast = pUnicast->Next;
                //    }
                //    info += Glib::ustring::sprintf("\tNumber of Unicast Addresses: %d\n", i);
                //} 
                //else {
                //    info += "\tNo Unicast Addresses\n";
                //}
                //pAnycast = pCurrAddresses->FirstAnycastAddress;
                //if (pAnycast) {
                //    for (i = 0; pAnycast != NULL; ++i) {
                //        info += "\tAny " + decodeAddr(&pAnycast->Address) + "\n"; 
                //        pAnycast = pAnycast->Next;
                //    }
                //    info += Glib::ustring::sprintf("\tNumber of Anycast Addresses: %d\n", i);
                //} else {
                //    info += Glib::ustring::sprintf("\tNo Anycast Addresses\n");
                //}

                //pMulticast = pCurrAddresses->FirstMulticastAddress;
                //if (pMulticast) {
                //    for (i = 0; pMulticast != NULL; ++i) {
                //        info += "\tMulti " + decodeAddr(&pMulticast->Address) + "\n"; 
                //        pMulticast = pMulticast->Next;
                //    }
                //    info += Glib::ustring::sprintf("\tNumber of Multicast Addresses: %d\n", i);
                //} else {
                //    info += "\tNo Multicast Addresses\n";
                //}

                pDnServer = pCurrAddresses->FirstDnsServerAddress;
                //if (pDnServer) {
                //    for (i = 0; pDnServer != NULL; i++)
                //        pDnServer = pDnServer->Next;
                //    info += Glib::ustring::sprintf("\tNumber of DNS Server Addresses: %d\n", i);
                //} else {
                //    info += Glib::ustring::sprintf("\tNo DNS Server Addresses\n");
                //}

                //info += Glib::ustring::sprintf("\tDNS Suffix: %s\n", StringUtils::utf8_encode(pCurrAddresses->DnsSuffix));                
                //info += Glib::ustring::sprintf("\tDescription: %s\n", StringUtils::utf8_encode(pCurrAddresses->Description));
                //info += Glib::ustring::sprintf("\FriendlyName: %s\n", StringUtils::utf8_encode(pCurrAddresses->FriendlyName));
                info += StringUtils::utf8_encode(pCurrAddresses->Description);

                //if (pCurrAddresses->PhysicalAddressLength != 0) {
                //    info += "\tPhysical address: ";
                //    for (i = 0; i < pCurrAddresses->PhysicalAddressLength;
                //         i++) {
                //        if (i == (pCurrAddresses->PhysicalAddressLength - 1))
                //            info += Glib::ustring::sprintf("%.2X\n",
                //                   (int) pCurrAddresses->PhysicalAddress[i]);
                //        else
                //            info += Glib::ustring::sprintf("%.2X-",
                //                   (int) pCurrAddresses->PhysicalAddress[i]);
                //    }
                //}
                //info += Glib::ustring::sprintf("\tFlags: %ld\n", pCurrAddresses->Flags);
                //info += Glib::ustring::sprintf("\tMtu: %lu\n", pCurrAddresses->Mtu);
                //info += Glib::ustring::sprintf("\tIfType: %ld\n", pCurrAddresses->IfType);
                //info += Glib::ustring::sprintf("\tOperStatus: %d\n", pCurrAddresses->OperStatus);
                if (pCurrAddresses->OperStatus >= 1) {
                    info += " up ";
                    double speed = pCurrAddresses->TransmitLinkSpeed;
                    std::string unit;
                    if (speed >= 1.0e9) {
                        speed /= 1.0e9;
                        unit = "G";
                    }
                    else if (speed >= 1.0e6) {
                        speed /= 1.0e6;
                        unit = "M";
                    }
                    else if (speed >= 1.0e3) {
                        speed /= 1.0e3;
                        unit = "k";
                    }
                    info += Glib::ustring::sprintf(" %.1lf%sBit/s", speed, unit);
                    //info += Glib::ustring::sprintf("\tTransmit link speed: %llu\n", pCurrAddresses->TransmitLinkSpeed);
                    //info += Glib::ustring::sprintf("\tReceive link speed: %llu\n", pCurrAddresses->ReceiveLinkSpeed);
                }
                else {
                    info += " down ";
                }
                //info += Glib::ustring::sprintf("\tIpv6IfIndex (IPv6 interface): %lu\n",
                //       pCurrAddresses->Ipv6IfIndex);
                //info += Glib::ustring::sprintf("\tZoneIndices (hex): ");
                //for (i = 0; i < 16; i++) {
                //    info += Glib::ustring::sprintf("%lx ", pCurrAddresses->ZoneIndices[i]);
                //}
                //info += "\n";


                //pPrefix = pCurrAddresses->FirstPrefix;
                //if (pPrefix) {
                //    for (i = 0; pPrefix != NULL; i++)
                //        pPrefix = pPrefix->Next;
                //    info += Glib::ustring::sprintf("\tNumber of IP Adapter Prefix entries: %d\n", i);
                //} 
                //else {
                //    info += Glib::ustring::sprintf("\tNumber of IP Adapter Prefix entries: 0\n");
                //}
                //info += "\n";

                pCurrAddresses = pCurrAddresses->Next;

                break;  // stop after found
            }
        }
    } 
    else {
        info += Glib::ustring::sprintf("Call to GetAdaptersAddresses failed with error: %lu", dwRetVal);
        if (dwRetVal == ERROR_NO_DATA)
            info += " No addresses were found for the requested parameters";
        else {
            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                    NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   
                    // Default language
                    (LPTSTR) & lpMsgBuf, 0, NULL)) {
                info += Glib::ustring::sprintf(" Error: %s", (char *)lpMsgBuf);
                LocalFree(lpMsgBuf);
            }
        }
    }
    if (pAddresses) {
        FREE(pAddresses);
    }    
    return info;
#pragma GCC diagnostic pop
}

std::string
SysInfoWindows::netInfo()
{    
    std::string info;
// It is possible for an adapter to have multiple
// IPv4 addresses, gateways, and secondary WINS servers
// assigned to the adapter. 
//
// Note that this sample code only prints out the 
// first entry for the IP address/mask, and gateway, and
// the primary and secondary WINS server for each adapter. 

    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;

/* variables used to print DHCP time info */
    //struct tm newtime;
    //char buffer[32];
    //errno_t error;

    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        return "Error allocating memory needed to call GetAdaptersinfo";
    }
// Make an initial call to GetAdaptersInfo to get
// the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            return "Error allocating memory needed to call GetAdaptersinfo";
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        info.reserve(64);
        while (pAdapter) {
            //printf("\tComboIndex: \t%lu\n", pAdapter->ComboIndex);
            //printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
            //printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
            //printf("\tAdapter Addr: \t"); // MAC
            //for (i = 0; i < pAdapter->AddressLength; i++) {
            //    if (i == (pAdapter->AddressLength - 1))
            //        printf("%.2X\n", (int) pAdapter->Address[i]);
            //    else
            //        printf("%.2X-", (int) pAdapter->Address[i]);
            //}
            //printf("\tIndex: \t%lu\n", pAdapter->Index);
            //printf("\tType: \t");
            //switch (pAdapter->Type) {
            //case MIB_IF_TYPE_OTHER:
            //    printf("Other\n");
            //    break;
            //case MIB_IF_TYPE_ETHERNET:
            //    printf("Ethernet\n");
            //    break;
            //case MIB_IF_TYPE_TOKENRING:
            //    printf("Token Ring\n");
            //    break;
            //case MIB_IF_TYPE_FDDI:
            //    printf("FDDI\n");
            //    break;
            //case MIB_IF_TYPE_PPP:
            //    printf("PPP\n");
            //    break;
            //case MIB_IF_TYPE_LOOPBACK:
            //    printf("Loopback\n");
            //    break;
            //case MIB_IF_TYPE_SLIP:
            //    printf("Slip\n");
            //    break;
            //default:
            //    printf("Unknown type %u\n", pAdapter->Type);
            //    break;
            //}
            info += netAdapterInfo(pAdapter->Index);
            info += "\n";
            info += pAdapter->IpAddressList.IpAddress.String;
            info += "/";
            info += pAdapter->IpAddressList.IpMask.String;
            //printf("\tIP Address: \t%s Ctx %lx\n",
            //       pAdapter->IpAddressList.IpAddress.String, pAdapter->IpAddressList.Context);
            //printf("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);

            //printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
            //printf("\t***\n");

            if (pAdapter->DhcpEnabled) {
                //printf("\tDHCP Enabled: Yes\n");
                //printf("\t  DHCP Server: \t%s\n",
                //       pAdapter->DhcpServer.IpAddress.String);

                //printf("\t  Lease Obtained: ");
                /* Display local time */
                //error = _localtime32_s(&newtime, (__time32_t*) &pAdapter->LeaseObtained);
                //if (error)
                //    printf("Invalid Argument to _localtime32_s\n");
                //else {
                //    // Convert to an ASCII representation 
                //    error = asctime_s(buffer, 32, &newtime);
                //    if (error)
                //        printf("Invalid Argument to asctime_s\n");
                //    else
                //        /* asctime_s returns the string terminated by \n\0 */
                //        printf("%s", buffer);
                //}

                //printf("\t  Lease Expires:  ");
                //error = _localtime32_s(&newtime, (__time32_t*) &pAdapter->LeaseExpires);
                //if (error)
                //    printf("Invalid Argument to _localtime32_s\n");
                //else {
                //    // Convert to an ASCII representation 
                //    error = asctime_s(buffer, 32, &newtime);
                //    if (error)
                //        printf("Invalid Argument to asctime_s\n");
                //    else
                //        /* asctime_s returns the string terminated by \n\0 */
                //        printf("%s", buffer);
                //}
            } 
            else {
                //printf("\tDHCP Enabled: No\n");
            }

            if (pAdapter->HaveWins) {
                //printf("\tHave Wins: Yes\n");
                //printf("\t  Primary Wins Server:    %s\n",
                //       pAdapter->PrimaryWinsServer.IpAddress.String);
                //printf("\t  Secondary Wins Server:  %s\n",
                //       pAdapter->SecondaryWinsServer.IpAddress.String);
            } 
            else {
                //printf("\tHave Wins: No\n");
            }
            pAdapter = pAdapter->Next;
            //printf("\n");
            break;  // only show one (info may repeat)
        }
    } 
    else {
        info += Glib::ustring::sprintf("GetAdaptersInfo failed with error: %lu\n", dwRetVal);
    }
    if (pAdapterInfo) {
        FREE(pAdapterInfo);
    }
    
    return info;   
}