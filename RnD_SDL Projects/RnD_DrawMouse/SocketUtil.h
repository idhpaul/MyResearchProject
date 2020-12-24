#ifndef SOCKETUTIL_H
#define SOCKETUTIL_H

#include "Socket.h"
#include <string>

class SocketUtil
{
public:
    static bool bind(SOCKET sockfd, std::string ip, uint16_t port);
    static void setNonBlock(SOCKET fd);
    static void setBlock(SOCKET fd, int writeTimeout = 0);
    static void setReuseAddr(SOCKET fd);
    static void setReusePort(SOCKET sockfd);
    static void setNoDelay(SOCKET sockfd);
    static void setKeepAlive(SOCKET sockfd);
    static void setNoSigpipe(SOCKET sockfd);
    static void setSendBufSize(SOCKET sockfd, int size);
    static void setRecvBufSize(SOCKET sockfd, int size);
    static void setRecvTimeout(SOCKET socfkd, int timeout);
    static void setLinger(SOCKET sockfd);
    static std::string getPeerIp(SOCKET sockfd);
    static uint16_t getPeerPort(SOCKET sockfd);
    static int getPeerAddr(SOCKET sockfd, struct sockaddr_in* addr);
    static void close(SOCKET sockfd);
    static bool connect(SOCKET sockfd, std::string ip, uint16_t port, int timeout = 0);
};

#endif
