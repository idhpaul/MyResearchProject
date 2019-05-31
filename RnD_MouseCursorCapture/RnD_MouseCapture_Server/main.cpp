#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include <WinSock2.h>
#pragma comment (lib, "Ws2_32.lib")

#include <Windows.h>



HCURSOR check_mouse_shape(char* statebuf) {
	CURSORINFO Info = { 0 };
	Info.cbSize = sizeof(Info);
	GetCursorInfo(&Info);

	if (Info.hCursor == (HCURSOR)0x0000000000010005) {
		sprintf(statebuf, "1_normal.cur");
	}
	else if (Info.hCursor == (HCURSOR)0x000000000001001b) {
		sprintf(statebuf, "2_run.cur");
	}
	else if (Info.hCursor == (HCURSOR)0x0000000000010007) {
		sprintf(statebuf, "3_IBeam.cur");
	}
	else if (Info.hCursor == (HCURSOR)0x0000000000010021) {
		sprintf(statebuf, "4_handclick.cur");
	}
	else if (Info.hCursor == (HCURSOR)0x0000000000010015) {
		sprintf(statebuf, "5_90arrow.cur");
	}
	else if (Info.hCursor == (HCURSOR)0x0000000000010013) {
		sprintf(statebuf, "6_180arrow.cur");
	}
	else if (Info.hCursor == (HCURSOR)0x000000000001000f) {
		sprintf(statebuf, "7_45arrow.cur");
	}
	else if (Info.hCursor == (HCURSOR)0x0000000000010011) {
		sprintf(statebuf, "8_135arrow.cur");
	}

	return Info.hCursor;
}

int main() {

	WSADATA wsadata;
	int opt = 0;

	SOCKET sock;
	SOCKET clientsock;

	struct sockaddr_in sockinfo;
	struct sockaddr_in clientsockinfo;

	int clientsize;



	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		printf("windows socket initialize failed\n");
		return -1;
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		printf("failed to create socket");
		return -1;
	}


	memset(&sockinfo, 0, sizeof(sockinfo));

	sockinfo.sin_family = AF_INET;
	sockinfo.sin_addr.s_addr = htonl(INADDR_ANY);
	sockinfo.sin_port = htons(9999);

	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));

	if (bind(sock, (SOCKADDR*)&sockinfo, sizeof(sockinfo)) == SOCKET_ERROR) {
		printf("PARMS bind() error");
		return -1;
	}

	if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
		printf("PARMS listen() error");
		return -1;
	}


	printf("Waiting for Client\n");

	clientsize = sizeof(clientsockinfo);

	clientsock = accept(sock, (SOCKADDR*)&clientsockinfo, &clientsize);
	if (clientsock == INVALID_SOCKET) {
		printf("PARMS accept() error");
		return -1;
	}
	closesocket(sock);

	//////////////////////////////////////////////////////////////////////

	int iResult;

	int iSendResult;
	char recvbuf[512];
	int recvbuflen = 512;

	char sendbuf[64] = "recv OK!";

	// Receive until the peer shuts down the connection
	do {

		ZeroMemory(recvbuf, sizeof(recvbuf));

		iResult = recv(clientsock, recvbuf, recvbuflen, 0);
		if (iResult > 0) {

			recvbuf[iResult] = '\0';

			printf("Bytes received size: %d \n\t\tdata: %s\n", iResult, recvbuf);

			char mousestatebuf[64];
			ZeroMemory(mousestatebuf, sizeof(mousestatebuf));

			HCURSOR cursor_shape = check_mouse_shape(mousestatebuf);
			printf("%p\n", cursor_shape);
			printf("mouse state:%s\n", mousestatebuf);

			// Echo the buffer back to the sender
			//iSendResult = send(clientsock, sendbuf, (int)strlen(sendbuf), 0);
			//iSendResult = send(clientsock, sendbuf, sizeof(sendbuf), 0);
			iSendResult = send(clientsock, mousestatebuf, sizeof(mousestatebuf), 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(clientsock);
				WSACleanup();
				return 1;
			}

		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(clientsock);
			WSACleanup();
			return 1;
		}

		if (!strcmp(recvbuf, "exit"))
			break;

	} while (iResult > 0);

	//////////////////////////////////////////////////////////////////////



	closesocket(clientsock);

	WSACleanup();

	printf("end program\n");

	return 0;
}