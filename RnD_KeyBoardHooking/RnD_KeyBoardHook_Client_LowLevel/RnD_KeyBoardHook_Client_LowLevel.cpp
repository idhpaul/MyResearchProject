#ifndef UNICODE
#define UNICODE 1
#endif

#include "pch.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#define __BSD_STYLE 1

typedef struct message
{
	char option[10];
	char user[20];
	char buf[1024];
	char target[256];
	int num1;
	int num2;
}MyStruct;

typedef struct HOOK {
	DWORD   vkCode;
	DWORD   scanCode;
	DWORD   flags;
	DWORD   time;
	ULONG_PTR dwExtraInfo;
	bool keyPress;
	bool controlPress;
	bool shiftPress;
	bool altPress;
	bool winPress;
} MYHOOK;

int __cdecl main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	//char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Validate the parameters
	/*if (argc != 2) {
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}*/

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//// Resolve the server address and port
	//iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	//if (iResult != 0) {
	//	printf("getaddrinfo failed with error: %d\n", iResult);
	//	WSACleanup();
	//	return 1;
	//}

	// Resolve the server address and port
	iResult = getaddrinfo("192.168.0.14", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

#if __BSD_STYLE
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}
#else
		/* Window Socket 참고 문서 : https://docs.microsoft.com/ko-kr/windows/desktop/api/winsock2/nf-winsock2-wsasocketw
				SOCKET WSAAPI WSASocketW(
					int                 af,
					int                 type,
					int                 protocol,
					LPWSAPROTOCOL_INFOW lpProtocolInfo,
					GROUP               g,
					DWORD               dwFlags
				);
		*/

		ConnectSocket = WSASocket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol, NULL, 0, 0);
#endif

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	else
	{
		printf("connect\n");
	}

	MYHOOK hookstruct = { 0, };

	BYTE keys[256] = { 0, };

	while (true)
	{

		ZeroMemory(keys, 256);

		//iResult = recv(ConnectSocket, (char*)&hookstruct, sizeof(MYHOOK), 0);
		iResult = recv(ConnectSocket, (char*)keys, sizeof(keys), 0);

		if (iResult > 0)
		{

			for (int i = 0; i < 256; ++i)
			{
				if (keys[i])
				{
					printf("[%x] key pressed\n", i);
				}
				//printf("%d ", keys[i]);
			}


			/*std::cout << "Bytes received: " << iResult << std::endl;
			std::cout << "keysate :" << hookstruct.keyPress
				<< "\t vkcode : " << hookstruct.vkCode
				<< "\t flag : " << hookstruct.flags
				<< "\t IsControlPress : " << hookstruct.controlPress
				<< "\t IsShiftPress : " << hookstruct.shiftPress
				<< "\t IsAltPress : " << hookstruct.altPress
				<< "\t IsWinPress : " << hookstruct.winPress << std::endl;*/

				//printf("Bytes received: %d\n", iResult);
				//printf("vkcode : %x \t flag : %x \t keysate\n", hookstruct.vkCode,hookstruct.flags);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		printf("///////////////////////////////////////////\n");
	}

	// Send an initial buffer
	/*iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}*/

	printf("Bytes Sent: %ld\n", iResult);

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive until the peer closes the connection
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}