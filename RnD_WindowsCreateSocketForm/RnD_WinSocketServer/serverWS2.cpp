
/*
	* 	C/C++ -> ��ó���� -> ��ó���� ���� / ������ ��ӵ� �� Ȯ��
	*	�����Ӽ� -> �Ϲ� -> �������տ��� ���ϴ� �� Ȯ��

	*	const char error �ذ�� : C/C++ -> ��� -> �ؼ���� / `�� progressive` ���� `�ƴϿ�`�� ���� (�����ؼ� ���õ� �κ���)

	���� ���� ����Ʈ : https://docs.microsoft.com/en-us/windows/desktop/winsock/complete-server-code

	��� : https://docs.microsoft.com/en-us/windows/desktop/api/winsock2/

	�����ڵ� : https://docs.microsoft.com/en-us/windows/desktop/api/winsock/nf-winsock-shutdown

*/
#ifndef UNICODE
#define UNICODE 1
#endif

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "9000"

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


int __cdecl main(void)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

#if __BSD_STYLE
	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
#else
	/* Window Socket ���� ���� : https://docs.microsoft.com/ko-kr/windows/desktop/api/winsock2/nf-winsock2-wsasocketw

				SOCKET WSAAPI WSASocketW(
					int                 af,
					int                 type,
					int                 protocol,
					LPWSAPROTOCOL_INFOW lpProtocolInfo,
					GROUP               g,
					DWORD               dwFlags
				);
		*/

	ListenSocket = WSASocket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol, NULL, 0, 0);
#endif

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// No longer need server socket
	closesocket(ListenSocket);

	//MyStruct mystruct;
	//ZeroMemory(&mystruct, sizeof(MyStruct));


	////mystruct.user = "DongHyun";
	////mystruct.target = "Window";
	////mystruct.option = "Fast";
	//strcpy(mystruct.user, "DongHyun");
	//strcpy(mystruct.target, "Window");
	//strcpy(mystruct.option, "Fast");
	//strcpy(mystruct.buf, "This is for test struct send message");
	//mystruct.num1 = 1;
	//mystruct.num2 = 2;

	//// Send an initial buffer by struct
	//iResult = send(ClientSocket, (char*)&mystruct, sizeof(MyStruct), 0);
	//if (iResult == SOCKET_ERROR) {
	//	printf("send failed with error: %d\n", WSAGetLastError());
	//	closesocket(ClientSocket);
	//	WSACleanup();
	//	return 1;
	//}

	//printf("Bytes Sent: %ld\n", iResult);

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {

			printf("recv data : %s", recvbuf);
			//printf("Bytes received: %d\n", iResult);

			//// Echo the buffer back to the sender
			//iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			//if (iSendResult == SOCKET_ERROR) {
			//	printf("send failed with error: %d\n", WSAGetLastError());
			//	closesocket(ClientSocket);
			//	WSACleanup();
			//	return 1;
			//}
			//printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}