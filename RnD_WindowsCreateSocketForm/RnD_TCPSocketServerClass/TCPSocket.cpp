#include "TCPSocket.h"
#include "SocketUtil.h"
#include <iostream>

#if defined(WIN32) || defined(_WIN32) 
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")
#endif 

TCPSocket::TCPSocket(SOCKET sockfd)
    : _sockfd(sockfd)
{
}

TCPSocket::~TCPSocket()
{
}

SOCKET TCPSocket::create()
{
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    return _sockfd;
}

bool TCPSocket::bind(std::string ip, uint16_t port)
{
    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    if (::bind(_sockfd, (struct sockaddr*) & addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cerr << "socket = " << _sockfd
            << " bind failed - IP : " << ip.c_str() << " Port : " << port << std::endl;

        return false;
    }

    return true;
}

bool TCPSocket::listen(int backlog)
{
    if (::listen(_sockfd, backlog) == SOCKET_ERROR)
    {
        std::cerr << "socket = " << _sockfd
            << "listen failed" << std::endl;

        return false;
    }

    return true;
}

SOCKET TCPSocket::accept()
{
    struct sockaddr_in addr = { 0 };
    socklen_t addrlen = sizeof addr;

    SOCKET clientfd = ::accept(_sockfd, (struct sockaddr*) & addr, &addrlen);

    return clientfd;
}

bool TCPSocket::connect(std::string ip, uint16_t port, int timeout)
{
    if (!SocketUtil::connect(_sockfd, ip, port, timeout))
    {
        std::cerr << "socket = " << _sockfd
            << "connect failed" << std::endl;
        
        return false;
    }

    return true;
}

void TCPSocket::close()
{
    closesocket(_sockfd);

    _sockfd = 0;
}

void TCPSocket::shutdownWrite()
{
    shutdown(_sockfd, SHUT_WR);
    _sockfd = 0;
}
