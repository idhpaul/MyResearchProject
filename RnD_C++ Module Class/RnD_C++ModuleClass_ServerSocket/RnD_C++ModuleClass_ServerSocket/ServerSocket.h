#define __SERVERSOCKET_CLASS
#ifdef __SERVERSOCKET_CLASS

/*
	Class Name : ServerSocket
	Author : IDH
*/

#include <iostream>
#include <sstream>
#include <vector>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <initializer_list>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define _GET_ADDR_VER 0

class ServerSocket
{
public:
	ServerSocket();
	ServerSocket(int socket_ea);

	~ServerSocket();

private:
	// Port EA
	int num_ports;

public:

	// TX ListenSocket List
	std::vector<SOCKET> ListenSocket_vec;
	// TX ClientSocket List
	std::vector<SOCKET> ClientSocket_vec;

	// Initailize TX Socket
	int socket_init(std::initializer_list<int> a_args);

	// Close TX Socket
	int socket_close();

};

ServerSocket::ServerSocket()
{
	std::cout << "[Class] TxSocket()" << std::endl;

	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cout << "WSAStartup failed with error: " << iResult << std::endl;
	}

	//Initialize Port EA
	num_ports = 10;

	//Reserve vector
	ListenSocket_vec.reserve(10);
	ClientSocket_vec.reserve(10);

	std::cout << "Default Make reserve " << num_ports << " ports" << std::endl;

}

ServerSocket::ServerSocket(int socket_ea) : ServerSocket::ServerSocket()
{
	std::cout << "[Class] TxSocket(int socket_ea)" << std::endl;


	num_ports = socket_ea;

	std::vector<SOCKET>().swap(ListenSocket_vec);
	std::vector<SOCKET>().swap(ClientSocket_vec);

	ListenSocket_vec.reserve(socket_ea);
	ClientSocket_vec.reserve(socket_ea);

	for (int i = 0; i < ListenSocket_vec.capacity(); i++)
	{
		ListenSocket_vec.push_back(INVALID_SOCKET);
	}
	for (int i = 0; i < ClientSocket_vec.capacity(); i++)
	{
		ClientSocket_vec.push_back(INVALID_SOCKET);
	}

}

ServerSocket::~ServerSocket()
{
	std::cout << "[Class] ~TxSocket()" << std::endl;

	this->socket_close();

	WSACleanup();
}

// //Initailize TX Socket
int ServerSocket::socket_init(std::initializer_list<int> a_args)
{

	int iResult;
	int arr_num = 0;

	if (num_ports == 0)
	{
		std::cout << "Not reserve vector" << std::endl;
		std::cout << "vector 크기 할당이 안되었습니다." << std::endl;
		return 1;
	}

	if (num_ports < a_args.size())
	{
		std::cout << "Many Input, reserve size : " << num_ports <<
			" Input size : " << a_args.size() << std::endl;
		std::cout << "생성된 크기보다 인자가 많습니다." << std::endl;
		return 1;
	}
	else if (num_ports > a_args.size())
	{
		std::cout << "More Input, reserve size : " << num_ports <<
			" Input size : " << a_args.size() << std::endl;
		std::cout << "생성된 크기보다 인자가 적습니다.." << std::endl;
		return 1;
	}

	//TODO: [추가 예외상황 처리] 인자의 포트번호가 겹칠경우 _ IDH _ 20190906


	for (auto port_number : a_args)
	{

		std::cout << "Start Make Socket Port : " << port_number << std::endl;

#if _GET_ADDR_VER
		std::ostringstream oss_port_num;
		oss_port_num << port_number;

		struct addrinfo *result = NULL;
		struct addrinfo hints;

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the server address and port
		iResult = getaddrinfo(NULL, oss_port_num.str().c_str(), &hints, &result);
		if (iResult != 0) {
			printf("getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			return 1;
		}

		// Create a SOCKET for connecting to server
		ListenSocket_vec[arr_num] = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket_vec[arr_num] == INVALID_SOCKET) {
			std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		// Setup the TCP listening socket
		iResult = bind(ListenSocket_vec[arr_num], result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(ListenSocket_vec[arr_num]);
			WSACleanup();
			return 1;
		}

		freeaddrinfo(result);

		iResult = listen(ListenSocket_vec[arr_num], SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket_vec[arr_num]);
			WSACleanup();
			return 1;
		}

		// Accept a client socket
		ClientSocket_vec[arr_num] = accept(ListenSocket_vec[arr_num], NULL, NULL);
		if (ClientSocket_list_arr[arr_num] == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket_vec[arr_num]);
			WSACleanup();
			return 1;
		}
#else

		int opt = 0;
		struct sockaddr_in sockinfo;
		struct sockaddr_in clientsockinfo;

		memset(&sockinfo, 0, sizeof(sockinfo));

		// Create a SOCKET for connecting to server
		ListenSocket_vec[arr_num] = socket(PF_INET, SOCK_STREAM, 0);
		if (ListenSocket_vec[arr_num] == INVALID_SOCKET)
		{
			std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return 1;
		}

		sockinfo.sin_family = AF_INET;
		sockinfo.sin_addr.s_addr = htonl(INADDR_ANY);
		sockinfo.sin_port = htons(port_number);

		setsockopt(ListenSocket_vec[arr_num], IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));

		// Setup the TCP listening socket
		iResult = bind(ListenSocket_vec[arr_num], (SOCKADDR*)&sockinfo, sizeof(sockinfo));
		if (iResult == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket_vec[arr_num]);
			WSACleanup();
			return 1;
		}

		iResult = listen(ListenSocket_vec[arr_num], SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket_vec[arr_num]);
			WSACleanup();
			return 1;
		}

		// Accept a client socket
		ClientSocket_vec[arr_num] = accept(ListenSocket_vec[arr_num], NULL, NULL);
		if (ClientSocket_vec[arr_num] == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket_vec[arr_num]);
			WSACleanup();
			return 1;
		}

#endif

		arr_num++;
		std::cout << "End Make Socket : " << port_number << std::endl;

	}

	return 0;
}

// //Close TX Socket
int ServerSocket::socket_close()
{
	for (int i = 0; i < ListenSocket_vec.size(); i++)
	{
		std::cout << "Close Listen Socket Number[" << i << "]" << std::endl;
		closesocket(ListenSocket_vec[i]);
		ListenSocket_vec[i] = INVALID_SOCKET;

	}

	for (int i = 0; i < ClientSocket_vec.size(); i++)
	{
		std::cout << "Close Listen Socket Number[" << i << "]" << std::endl;
		closesocket(ClientSocket_vec[i]);
		ClientSocket_vec[i] = INVALID_SOCKET;

	}

	return 0;
}

#endif //__SERVERSOCKET_CLASS






