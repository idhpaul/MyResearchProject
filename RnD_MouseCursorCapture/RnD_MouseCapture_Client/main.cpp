

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment (lib,"Ws2_32.lib")

#include <Windows.h>
#include <WinUser.h>

#include <stdio.h>



int main() {

	WSADATA wsadata;

	SOCKET clientsock;
	struct sockaddr_in clientsockinfo;

	int opt = 0;
	int recvlen = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		printf("windows socket initialize failed\n");
		return -1;
	}

	clientsock = socket(PF_INET, SOCK_STREAM, 0);
	if (clientsock == INVALID_SOCKET) {
		printf("failed to create socket");
		return -1;
	}

	memset(&clientsockinfo, 0, sizeof(clientsockinfo));

	clientsockinfo.sin_family = AF_INET;
	inet_pton(AF_INET, "192.168.0.3", &(clientsockinfo.sin_addr));
	clientsockinfo.sin_port = htons(9999);

	setsockopt(clientsock, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));


	if (connect(clientsock, (SOCKADDR*)&clientsockinfo, sizeof(clientsockinfo)) == SOCKET_ERROR) {
		printf("faile to connect");
		return -1;
	}

	///////////////////////////////////////////////////////////////////////////////////////

	//char *sendbuf = "this is a test";
	char recvbuf[512];
	int iResult;
	int recvbuflen = 512;

	char sendbuf[64] = "Communcation START";

	// Send an initial buffer
	iResult = send(clientsock, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(clientsock);
		WSACleanup();
		return 1;
	}

	char inputData[512] = "hi";
	char *p;

	HCURSOR def_arrow_cur = CopyCursor(LoadCursor(0, IDC_ARROW));

	// Receive until the peer closes the connection
	do {

		iResult = recv(clientsock, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			recvbuf[iResult] = '\0';
			printf("\t\t server mouse state : %s\n", recvbuf);

			//HCURSOR hcursor = LoadCursor(0, IDC_ARROW);
			//HCURSOR hcursor_copy = CopyCursor(hcursor);
			//BOOL ret = SetSystemCursor(hcursor_copy, 32649);


			SetSystemCursor(LoadCursorFromFileA(recvbuf), 32512);



			/*ZeroMemory(inputData, sizeof(inputData));

			printf("Input String : ");
			fgets(inputData, sizeof(inputData) - 1, stdin);
			if ((p = strchr(inputData, '\n')) != NULL)
				*p = '\0';

			printf("[%s]\n", inputData);*/

			// Send an initial buffer
			iResult = send(clientsock, inputData, (int)strlen(sendbuf), 0);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(clientsock);
				WSACleanup();
				return 1;
			}

			//DestroyCursor(hcursor);



		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

		if (!strcmp(inputData, "exit"))
			break;

	} while (iResult > 0);

	SetSystemCursor(def_arrow_cur, 32512);


	///////////////////////////////////////////////////////////////////////////////////////

	// shutdown the connection since no more data will be sent
	iResult = shutdown(clientsock, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(clientsock);
		WSACleanup();
		return 1;
	}


	closesocket(clientsock);

	WSACleanup();

	printf("program end...\n");

	return 0;
}