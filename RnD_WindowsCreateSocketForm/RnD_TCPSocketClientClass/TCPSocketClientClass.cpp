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

#define SEND_TEST 1
#define RECV_TEST 0


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

	std::string ip = "192.168.0.55";
	uint16_t port = 23654;

	uint32_t send_buf_size = 512;
	std::shared_ptr<char> send_buf(new char[send_buf_size], std::default_delete<char[]>());
	uint32_t recv_buf_size = 512;
	std::shared_ptr<char> rev_buf(new char[recv_buf_size], std::default_delete<char[]>());

	std::shared_ptr<TcpSocket> tcpSocket(new TcpSocket);
	SOCKET clientSocketfd;

	tcpSocket->Create();

	bResult = tcpSocket->Connect(ip, port);
	if (FALSE == bResult)
	{
		std::cerr << "main tcpSocket connect failed" << std::endl;
		std::cerr << "GetLastError : " << WSAGetLastError() << std::endl;

		goto EXIT;
	}

#if SEND_TEST
	strcpy(send_buf.get(), "Hello Im RX, This is send test bye!\n");

	// TODO : What is diff send return values
	ret = ::send(tcpSocket->GetSocket(), send_buf.get(), send_buf_size, 0);
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

	ret = ::recv(tcpSocket->GetSocket(), recv_buf.get(), send_buf_size, 0);
	if (ret == -1)
	{
		// Error Handling;
		std::cout << "Error Handling Please" << std::endl;
		goto EXIT;
	}

	else if (ret == 0)
	{
		std::cout << "This socket is dead. close this socket" << std::endl;
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

		std::cout << "[RECV message] \n\t" << recvbuf << std::endl;
	}
#endif

EXIT:

	std::cout << WSAGetLastError() << std::endl;


	tcpSocket->Close();
	tcpSocket.reset();
	tcpSocket = nullptr;

	// Unload Windows Socket DLL
	WSACleanup();

	return 0;
}