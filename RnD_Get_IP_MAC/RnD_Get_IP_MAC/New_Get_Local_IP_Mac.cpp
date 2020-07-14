
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN						// Do not include external MFC overheader
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <Windows.h>

#include <cstdint>
#include <string>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")

bool GetLocalIPAddress(std::string& localIPAddr, std::string& localMacAddr)
{
#if defined(__linux) || defined(__linux__) 
    SOCKET sockfd = 0;
    char buf[512] = { 0 };
    struct ifconf ifconf;
    struct ifreq* ifreq;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET)
    {
        close(sockfd);
        return "0.0.0.0";
    }

    ifconf.ifc_len = 512;
    ifconf.ifc_buf = buf;
    if (ioctl(sockfd, SIOCGIFCONF, &ifconf) < 0)
    {
        close(sockfd);
        return "0.0.0.0";
    }

    close(sockfd);

    ifreq = (struct ifreq*)ifconf.ifc_buf;
    for (int i = (ifconf.ifc_len / sizeof(struct ifreq)); i > 0; i--)
    {
        if (ifreq->ifr_flags == AF_INET)
        {
            if (strcmp(ifreq->ifr_name, "lo") != 0)
            {
                return inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr);
            }
            ifreq++;
        }
    }
    return "0.0.0.0";
#elif defined(WIN32) || defined(_WIN32)

    // Load Windows Socket DLL
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        std::cerr << "Win socket start failed" << std::endl;
        WSACleanup();
    }

    char* mac_addr = new char[18];

    PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
    unsigned long size = sizeof(IP_ADAPTER_INFO);

    int ret = GetAdaptersInfo(pIpAdapterInfo, &size);
    if (ret == ERROR_BUFFER_OVERFLOW)
    {
        delete pIpAdapterInfo;
        pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[size];
        ret = GetAdaptersInfo(pIpAdapterInfo, &size);
    }

    if (ret != ERROR_SUCCESS)
    {
        delete pIpAdapterInfo;
        return false;
    }

    while (pIpAdapterInfo)
    {
        IP_ADDR_STRING* pIpAddrString = &(pIpAdapterInfo->IpAddressList);
        while (pIpAddrString)
        {
            if (strcmp(pIpAddrString->IpAddress.String, "127.0.0.1") != 0
                && strcmp(pIpAddrString->IpAddress.String, "0.0.0.0") != 0
                && strcmp(pIpAdapterInfo->GatewayList.IpAddress.String, "0.0.0.0") != 0)
            {
                // pIpAddrString->IpMask.String 
                //pIpAdapterInfo->GatewayList.IpAddress.String

                std::string ip(pIpAddrString->IpAddress.String);
                localIPAddr = ip;

                sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
                    pIpAdapterInfo->Address[0], pIpAdapterInfo->Address[1],
                    pIpAdapterInfo->Address[2], pIpAdapterInfo->Address[3],
                    pIpAdapterInfo->Address[4], pIpAdapterInfo->Address[5]);

                localMacAddr = mac_addr;

                break;
            }
            pIpAddrString = pIpAddrString->Next;
        } while (pIpAddrString);
        pIpAdapterInfo = pIpAdapterInfo->Next;
    }

    delete pIpAdapterInfo;
    delete mac_addr;

    WSACleanup();

    return false;
#else
    return false;
#endif
}

int main()
{
    std::string ipAddr;
    std::string macAddr;

    GetLocalIPAddress(ipAddr, macAddr);

    return 0;
}