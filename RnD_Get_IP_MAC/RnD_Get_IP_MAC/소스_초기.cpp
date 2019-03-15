//#include <stdio.h>
//#include <WinSock.h>
//#include <IPHlpApi.h>                       // for GetAdaptersInfo()
//#pragma comment(lib, "iphlpapi.lib" )
//
//char my_mac[64];
//char my_ip[64];
//
//int main(int argc, char* argv[])
//{
//	DWORD size = sizeof(PIP_ADAPTER_INFO);
//
//	PIP_ADAPTER_INFO Info;
//	ZeroMemory(&Info, size);
//	int result = GetAdaptersInfo(Info, &size);        // ù��° ��ī�� MAC address ��������
//	if (result == ERROR_BUFFER_OVERFLOW)    // GetAdaptersInfo�� �޸𸮰� �����ϸ� �� �Ҵ��ϰ� ��ȣ��
//	{
//		Info = (PIP_ADAPTER_INFO)malloc(size);
//		GetAdaptersInfo(Info, &size);
//	}
//	if (!Info)        return 0;
//	sprintf(my_mac, "%0.2X:%0.2X:%0.2X:%0.2X:%0.2X:%0.2X",
//		Info->Address[0], Info->Address[1], Info->Address[2], Info->Address[3], Info->Address[4], Info->Address[5]);
//	sprintf(my_ip, "%s",
//		Info->IpAddressList.IpAddress.String);
//
//	printf(my_ip);
//	getchar();
//	return 1;
//}

/* ���� */
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

char my_ip[64];

int main()
{
	//ZeroMemory(my_ip, sizeof(my_ip));
	WSADATA wsaData; // WinSock ���࿡ �ʿ��� ����ü
	SOCKET ipSocket; // �� ��ǻ�Ͱ� ��Ʈ��ũ���� �ĺ��Ǵ� �̸��� ����� ���ڿ� �����Դϴ�.
	DWORD dwInterfaceInfo; // �� �迭���� ���� �˻��� ��ġ���� �����Դϴ�.
	DWORD dwIndex; // �迭 �ε���
	struct sockaddr_in sockaddr_info1, sockaddr_info2;
	int sockaddr_info_size = sizeof(struct sockaddr_in);
	char szBuffer[128];

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	sockaddr_info1.sin_family = AF_INET;
	sockaddr_info1.sin_port = htons(4567);
	sockaddr_info1.sin_addr.s_addr = inet_addr("8.8.8.8");

	ipSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	connect(ipSocket, (struct sockaddr*)&sockaddr_info1, sizeof sockaddr_info1);
	//getpeername(ipSocket, (struct sockaddr*)&sockaddr_info2, &sockaddr_info_size);
	//printf("���� IP %s:%u\n", inet_ntoa(sockaddr_info2.sin_addr), ntohs(sockaddr_info2.sin_port));
	getsockname(ipSocket, (struct sockaddr*)&sockaddr_info2, &sockaddr_info_size);
	//printf("%s\n", inet_ntoa(sockaddr_info2.sin_addr));
	sprintf(my_ip, "%s", inet_ntoa(sockaddr_info2.sin_addr));
	printf(my_ip);
	closesocket(ipSocket);
	WSACleanup();

	printf("END");
	_getch();

	return 0;
}