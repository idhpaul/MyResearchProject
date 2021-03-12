#pragma warning( disable : 4996) // disable _CRT_SECURE_NO_WARNINGS

#include "TcpSocket.h"
#include "SocketUtil.h"

#include <memory>
#include <iostream>

#define TASK_SCHEDULER_PRIORITY_LOW       0
#define TASK_SCHEDULER_PRIORITY_NORMAL    1
#define TASK_SCHEDULER_PRIORITYO_HIGH     2 
#define TASK_SCHEDULER_PRIORITY_HIGHEST   3
#define TASK_SCHEDULER_PRIORITY_REALTIME  4

#define SEND_TEST 0
#define RECV_TEST 1

std::string GetLocalIPAddress()
{
#if defined(__linux) || defined(__linux__) 
    SOCKET sockfd = 0;
    char buf[512] = { 0 };
    struct ifconf ifconf;
    struct ifreq* ifreq;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET)
    {
        close(sockfd);
        return "0.0.0.0";
    }

    ifconf.ifc_len = 512;
    ifconf.ifc_buf = buf;
    if (ioctl(sockfd, SIOCGIFCONF, &ifconf) < 0)
    {
        close(sockfd);
        return "0.0.0.0";
    }

    close(sockfd);

    ifreq = (struct ifreq*)ifconf.ifc_buf;
    for (int i = (ifconf.ifc_len / sizeof(struct ifreq)); i > 0; i--)
    {
        if (ifreq->ifr_flags == AF_INET)
        {
            if (strcmp(ifreq->ifr_name, "lo") != 0)
            {
                return inet_ntoa(((struct sockaddr_in*) & (ifreq->ifr_addr))->sin_addr);
            }
            ifreq++;
        }
    }
    return "0.0.0.0";
#elif defined(WIN32) || defined(_WIN32)
    PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
    unsigned long size = sizeof(IP_ADAPTER_INFO);

    int ret = GetAdaptersInfo(pIpAdapterInfo, &size);
    if (ret == ERROR_BUFFER_OVERFLOW)
    {
        delete pIpAdapterInfo;
        pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[size];
        ret = GetAdaptersInfo(pIpAdapterInfo, &size);
    }

    if (ret != ERROR_SUCCESS)
    {
        delete pIpAdapterInfo;
        return "0.0.0.0";
    }

    while (pIpAdapterInfo)
    {
        IP_ADDR_STRING* pIpAddrString = &(pIpAdapterInfo->IpAddressList);
        while (pIpAddrString)
        {
            if (strcmp(pIpAddrString->IpAddress.String, "127.0.0.1") != 0
                && strcmp(pIpAddrString->IpAddress.String, "0.0.0.0") != 0)
            {
                // pIpAddrString->IpMask.String 
                //pIpAdapterInfo->GatewayList.IpAddress.String
                std::string ip(pIpAddrString->IpAddress.String);
                //delete pIpAdapterInfo;
                return ip;
            }
            pIpAddrString = pIpAddrString->Next;
        } while (pIpAddrString);
        pIpAdapterInfo = pIpAdapterInfo->Next;
    }

    delete pIpAdapterInfo;
    return "0.0.0.0";
#else
    return "0.0.0.0";
#endif
}

int main()
{

	// Load Windows Socket DLL
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		std::cerr << "Win socket start failed" << std::endl;
		WSACleanup();
	}

	// Get console handle
	HWND hConsole = GetConsoleWindow();

	// Set Thread Priority
	int priority = TASK_SCHEDULER_PRIORITY_REALTIME;

	SetThreadDescription(GetCurrentThread(), L"main_thread");

	switch (priority)
	{
	case TASK_SCHEDULER_PRIORITY_LOW:
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
		break;
	case TASK_SCHEDULER_PRIORITY_NORMAL:
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
		break;
	case TASK_SCHEDULER_PRIORITYO_HIGH:
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
		break;
	case TASK_SCHEDULER_PRIORITY_HIGHEST:
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
		break;
	case TASK_SCHEDULER_PRIORITY_REALTIME:
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
		break;
	}

    bool bResult = false;
    int ret = 0;

	std::string ip = GetLocalIPAddress();
	uint16_t port = 23654;

    uint32_t send_buf_size = 512;
    std::shared_ptr<char> send_buf(new char[send_buf_size], std::default_delete<char[]>());
    uint32_t recv_buf_size = 512;
    std::shared_ptr<char> rev_buf(new char[recv_buf_size], std::default_delete<char[]>());

	std::shared_ptr<TcpSocket> tcpSocket(new TcpSocket);

	tcpSocket->Create();
	//SocketUtil::setReuseAddr(tcpSocket->fd());
	//SocketUtil::setReusePort(tcpSocket->fd());
	//SocketUtil::setNonBlock(tcpSocket->fd());
    //SocketUtil::setKeepAlive(tcpSocket->fd());

	bResult = tcpSocket->Bind(ip, port);
    if (FALSE == bResult)
    {
        std::cerr << "main tcpSocket bind failed" << std::endl;
        std::cerr << "GetLastError : " << WSAGetLastError() << std::endl;

        goto EXIT;
    }
    
    bResult = tcpSocket->Listen(SOMAXCONN);
    if (FALSE == bResult)
    {
        std::cerr << "main tcpSocket listen failed" << std::endl;
        std::cerr << "GetLastError : " << WSAGetLastError() << std::endl;

        goto EXIT;
    }

    std::cout << "Listening........" << std::endl;

    std::shared_ptr<TcpSocket> clientSocket(new TcpSocket(tcpSocket->Accept()));
    if (clientSocket == nullptr)
    {
        std::cerr << "main tcpSocket accept failed" << std::endl;
        std::cerr << "GetLastError : " << WSAGetLastError() << std::endl;

        goto EXIT;
    }

    std::cout << "Accept OK!" << std::endl;


#if SEND_TEST

    strcpy(sendbuf, "Hello Im TX, This is send test bye!\n");

    // TODO : What is diff send return values
    ret = ::send(clientSocket->GetSocket(), send_buf.get(), send_buf_size, 0);
    if (ret > 0)
    {
        std::cout << "Send OK - sendsize: " << ret << std::endl;
    }
    else if (ret < 0)
    {
        std::cout << "Send ret < 0" << std::endl;
#if defined(__linux) || defined(__linux__)
        if (errno == EINTR || errno == EAGAIN)
#elif defined(WIN32) || defined(_WIN32)
        int error = WSAGetLastError();
        if (error == WSAEWOULDBLOCK || error == WSAEINPROGRESS || error == 0)
#endif
            ret = 0;
    }
    else
    {
        std::cout << "Send ret = 0" << std::endl;
    }
#endif

#if RECV_TEST
    while (true)
    {
        ret = ::recv(clientSocket->GetSocket(), rev_buf.get(), recv_buf_size, 0);
        if (ret == -1)
        {
            // Error Handling;
            std::cout << "Error Recv return -1, Handling Please" << std::endl;
            std::cout << WSAGetLastError() << std::endl;
            goto EXIT;
        }
        else if (ret == 0)
        {
            std::cout << "Recv return 0, This socket is dead. close this socket" << std::endl;
            std::cout << WSAGetLastError() << std::endl;
            goto EXIT;
        }
        else // Recv OK
        {
            //if (ret < wantrecvsize) // This case can not receive hole data
            //{
            //      // Return Get Data
            //      return ret;
            //}
            //else
            std::cout << "All Received!" << std::endl;
            std::cout << WSAGetLastError() << std::endl;

            std::cout << "[RECV message] " << rev_buf.get() << std::endl;
        }
    }
#endif

EXIT:
    clientSocket->Close();
    tcpSocket->Close();

	// Unload Windows Socket DLL
	WSACleanup();

	return 0;
}