#pragma once
#ifndef SOCKET_H
#define SOCKET_H

#define WIN32_LEAN_AND_MEAN						// Do not include external MFC overheader
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#define SHUT_RD 0
#define SHUT_WR 1 
#define SHUT_RDWR 2

#include <cstdint>
#include <string>

#endif