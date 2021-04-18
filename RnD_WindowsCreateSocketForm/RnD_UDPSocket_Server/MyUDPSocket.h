#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>

#include <stdint.h>

#include <string>

#pragma comment(lib, "Ws2_32.lib")

enum udp_socket_type
{
	udp_server = 0,
	udp_client = 1
};

class MyUDPSocket
{
public:
	MyUDPSocket(enum udp_socket_type socket_type);
	~MyUDPSocket();

	int Handshake(std::string ip, uint16_t port);

private:
	const enum udp_socket_type socket_type_;

	// for server
	SOCKET   ServerSocket_;  
	SOCKADDR_IN  ServerInfo_;
	SOCKADDR_IN  FromClient_;   
	int   FromClient_Size_;

	// for client
	SOCKET   ClientSocket_;
	SOCKADDR_IN  ToServer_;   
	SOCKADDR_IN  FromServer_;

	uint16_t port_;
};
