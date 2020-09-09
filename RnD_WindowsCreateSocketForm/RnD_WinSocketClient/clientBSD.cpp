#include <stdio.h>

#include <WinSock2.h>

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
	clientsockinfo.sin_addr.s_addr = inet_addr("192.168.0.3");
	clientsockinfo.sin_port = htons(9999);

	setsockopt(clientsock, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));


	if (connect(clientsock, (SOCKADDR*)&clientsockinfo, sizeof(clientsockinfo)) == SOCKET_ERROR) {
		printf("faile to connect");
		return -1;
	}

	recvlen = recv(clientsock, "message buffer here", sizeof(20) - 1, 0);
	if (recvlen == -1)
		printf("faile to receive message\n");

	closesocket(clientsock);

	WSACleanup();

	return 0;
}