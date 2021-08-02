#include <stdio.h>


#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")

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

	while (true)
	{
		int res = send(clientsock, "message buffer here", sizeof(20), 0);
		if (res > 0)
		{

		}
		else if (res == 0)
		{
			printf("GetLastError : %d\n", WSAGetLastError());
		}
		else
		{
			printf("GetLastError : %d\n", WSAGetLastError());
		}

		Sleep(1000);
	}



	closesocket(sock);
	closesocket(clientsock);

	WSACleanup();

	return 0;
}