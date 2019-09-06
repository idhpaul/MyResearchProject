#define __CLIENTSOCKET_CLASS
#ifdef __CLIENTSOCKET_CLASS

/*
	Class Name : ClientSocket
	Author : IDH
*/

#include <iostream>
#include <sstream>
#include <vector>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <initializer_list>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define _GET_ADDR_VER 0

class ClientSocket
{
public:
	ClientSocket();
	ClientSocket(int socket_ea);

	~ClientSocket();

private:
	// Server Socket IP
	std::string server_ip;
	// Socket Port EA
	int num_ports;


public:
	// Client ConnectSocket List
	std::vector<SOCKET> ConnectSocket_vec;


	// Initalize Client Socket(for localhost)
	int socket_init(std::initializer_list<int> a_args);

	// Initailize Client Socket
	// ex) socket_init("192.168.0.3",{5,9000,9001,9002,9003,9004}) make 5 sockets, ports are 9000~9004
	int socket_init(std::string server_ip, std::initializer_list<int> a_args);

	// Close Client Socket
	int socket_close();

};

ClientSocket::ClientSocket()
{
	std::cout << "[Class] CleintSocket()" << std::endl;

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

	ConnectSocket_vec.reserve(10);

	std::cout << "Default Make reserve " << num_ports << " ports" << std::endl;

}

ClientSocket::ClientSocket(int socket_ea) : ClientSocket::ClientSocket()
{
	std::cout << "[Class] CleintSocket(int socket_ea)" << std::endl;

	//Initialize Port list and vector
	num_ports = socket_ea;

	std::vector<SOCKET>().swap(ConnectSocket_vec);

	ConnectSocket_vec.reserve(socket_ea);

	for (int i = 0; i < ConnectSocket_vec.capacity(); i++)
	{
		ConnectSocket_vec.push_back(INVALID_SOCKET);
	}


}

ClientSocket::~ClientSocket()
{
	std::cout << "[Class] ~ClientSocket()" << std::endl;

	this->server_ip.clear();

	this->socket_close();

	WSACleanup();
}

// Initalize Client Socket
int ClientSocket::socket_init(std::initializer_list<int> a_args)
{
	std::cout << "set Server ip for Localhost" << std::endl;

	this->socket_init("127.0.0.1", a_args);

	return 0;
}

// //Initailize TX Socket
int ClientSocket::socket_init(std::string server_ip, std::initializer_list<int> a_args)
{

	int iResult;
	int arr_num = 0;

	//Set Server IP
	this->server_ip = server_ip;
	if (this->server_ip.empty())
	{
		std::cout << "서버 IP를 입력하세요" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "Server IP : " << this->server_ip << std::endl;
	}


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
		struct addrinfo *ptr = NULL;
		struct addrinfo hints;


		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		iResult = getaddrinfo(server_ip.c_str(), oss_port_num.str().c_str(), &hints, &result);
		if (iResult != 0) {
			printf("getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			return 1;
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

			// Create a SOCKET for connecting to server
			ConnectSocket_vec[arr_num] = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (ConnectSocket_vec[arr_num] == INVALID_SOCKET) {
				printf("socket failed with error: %ld\n", WSAGetLastError());
				WSACleanup();
				return 1;
			}

			// Connect to server.
			iResult = connect(ConnectSocket_vec[arr_num], ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(ConnectSocket_vec[arr_num]);
				ConnectSocket_vec[arr_num] = INVALID_SOCKET;
				continue;
			}
			break;
		}
#else
		int opt = 0;
		struct sockaddr_in clientsockinfo;

		// Create a SOCKET for connecting to server
		ConnectSocket_vec[arr_num] = socket(PF_INET, SOCK_STREAM, 0);
		if (ConnectSocket_vec[arr_num] == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		memset(&clientsockinfo, 0, sizeof(clientsockinfo));

		clientsockinfo.sin_family = AF_INET;
		inet_pton(AF_INET, server_ip.c_str(), &clientsockinfo.sin_addr);
		//clientsockinfo.sin_addr.s_addr = server_ip.c_str();
		clientsockinfo.sin_port = htons(port_number);

		setsockopt(ConnectSocket_vec[arr_num], IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));

		// Connect to server.
		iResult = connect(ConnectSocket_vec[arr_num], (SOCKADDR*)&clientsockinfo, sizeof(clientsockinfo));
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket_vec[arr_num]);
			ConnectSocket_vec[arr_num] = INVALID_SOCKET;
			continue;
		}
#endif

		arr_num++;
		std::cout << "End Make Socket : " << port_number << std::endl;

	}

	return 0;
}

// //Close Client Socket
int ClientSocket::socket_close()
{
	for (int i = 0; i < ConnectSocket_vec.size(); i++)
	{
		std::cout << "Close Connect Socket Number[" << i << "]" << std::endl;
		closesocket(ConnectSocket_vec[i]);
		ConnectSocket_vec[i] = INVALID_SOCKET;
	}


	return 0;
}

#endif //__CLIENTSOCKET_CLASS