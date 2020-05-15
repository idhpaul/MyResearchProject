#pragma once
#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "Socket.h"

class TCPSocket
{
public:
    TCPSocket(SOCKET sockfd = -1);
    virtual ~TCPSocket();

    virtual SOCKET create();

    virtual bool bind(std::string ip, uint16_t port);
    virtual bool listen(int backlog);
    virtual SOCKET accept();

    virtual bool connect(std::string ip, uint16_t port, int timeout = 0);

    virtual void close();
    virtual void shutdownWrite();

    SOCKET fd() const { return _sockfd; }

private:
    SOCKET _sockfd = -1;
};
#endif //TCPSOCKET_H
