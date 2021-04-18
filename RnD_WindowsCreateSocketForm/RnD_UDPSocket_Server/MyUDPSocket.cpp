//error C4996 : 'inet_addr' : Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include "MyUDPSocket.h"

MyUDPSocket::MyUDPSocket(enum udp_socket_type socket_type)
	:socket_type_(socket_type)

{
}

MyUDPSocket::~MyUDPSocket()
{
}

int MyUDPSocket::Handshake(std::string ip, uint16_t port)
{
    if (socket_type_ == udp_server)
    {
        port_ = port;

        int iResult = 0;

        memset(&ServerInfo_, 0, sizeof(ServerInfo_));
        memset(&FromClient_, 0, sizeof(FromClient_));

        // Create a receiver socket to receive datagrams
        ServerSocket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (ServerSocket_ == INVALID_SOCKET)
        {
            wprintf(L"socket failed with error %d\n", WSAGetLastError());
            return -1;
        }
        //-----------------------------------------------
        // Bind the socket to any address and the specified port.
        ServerInfo_.sin_family = AF_INET;
        ServerInfo_.sin_port = htons(port_);
        ServerInfo_.sin_addr.s_addr = htonl(INADDR_ANY);

        iResult = bind(ServerSocket_, (struct sockaddr*)&ServerInfo_, sizeof(ServerInfo_));
        if (iResult != 0)
        {
            wprintf(L"bind failed with error %d\n", WSAGetLastError());
            return -1;
        }
        //-----------------------------------------------
        // Call the recvfrom function to receive datagrams
        // on the bound socket.

        char handshake_buf[64] = { 0, };

        int FromClient_Size_ = sizeof(FromClient_);

        wprintf(L"Receiving datagrams...\n");

        iResult = recvfrom(ServerSocket_,
            handshake_buf, 64, 0, (SOCKADDR*)&FromClient_, &FromClient_Size_);
        if (iResult == SOCKET_ERROR)
        {
            wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
        }
        else
        {
            // TODO: print client info
            // ref : https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo

            printf("recv from client handshake message : %s\n", handshake_buf);
            //print details of the client/peer and the data received
            printf("Received packet from %s:%d\n", inet_ntoa(FromClient_.sin_addr), ntohs(FromClient_.sin_port));

        }
        

        memset(handshake_buf, 0, 64);
        memcpy(handshake_buf, "hello client(handshake)", 24);

        wprintf(L"Sending a datagram to the receiver...\n");
        iResult = sendto(ServerSocket_,
            handshake_buf, 24, 0, (SOCKADDR*)&FromClient_, FromClient_Size_);
        if (iResult == SOCKET_ERROR) {
            wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
            closesocket(ServerSocket_);

            return -1;
        }
    }
    else
    {

        port_ = port;

        //create socket
        if ((ClientSocket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
        {
            wprintf(L"socket() failed with error code : %d", WSAGetLastError());

            return -1;
        }

        //setup address structure
        memset(&ToServer_, 0, sizeof(ToServer_));
        memset(&FromServer_, 0, sizeof(FromServer_));

        ToServer_.sin_family = AF_INET;
        ToServer_.sin_port = htons(port_);
        ToServer_.sin_addr.S_un.S_addr = inet_addr(ip.c_str());



        char handshake_buf[64] = { 0, };
        memcpy(handshake_buf, "hello server(handshake)", 24);

        //send the message
        if (sendto(ClientSocket_, handshake_buf, 24, 0, (struct sockaddr*)&ToServer_, sizeof(ToServer_)) == SOCKET_ERROR)
        {
            wprintf(L"sendto() failed with error code : %d", WSAGetLastError());

            return -1;
        }

        int FromServer_Size = sizeof(FromServer_);

        memset(handshake_buf, 0, 64);

        //try to receive some data, this is a blocking call
        if (recvfrom(ClientSocket_, handshake_buf, 64, 0, (struct sockaddr*)&FromServer_, &FromServer_Size) == SOCKET_ERROR)
        {
            wprintf(L"recvfrom() failed with error code : %d", WSAGetLastError());

            return -1;
        }
        else
        {
            // TODO: print server info
            // ref : https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo

            printf("recv from server handshake message : %s\n", handshake_buf);
            //print details of the client/peer and the data received
            printf("Received packet from %s:%d\n", inet_ntoa(FromServer_.sin_addr), ntohs(FromServer_.sin_port));
        }
    }

    
	return 0;
}
