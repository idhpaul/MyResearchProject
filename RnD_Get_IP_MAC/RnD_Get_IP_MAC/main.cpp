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
	WSADATA wsaData; // WinSock 실행에 필요한 구조체
	SOCKET ipSocket; // 이 컴퓨터가 네트워크에서 식별되는 이름이 저장될 문자열 버퍼입니다.
	DWORD dwInterfaceInfo; // 위 배열에서 실제 검색된 장치들의 개수입니다.
	DWORD dwIndex; // 배열 인덱스
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
	//printf("상대방 IP %s:%u\n", inet_ntoa(sockaddr_info2.sin_addr), ntohs(sockaddr_info2.sin_port));
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

//#include <winsock2.h>
//#include <iphlpapi.h>
//#include <stdio.h>
//#include <stdlib.h>
//#pragma comment(lib, "IPHLPAPI.lib")
//
//#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
//#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
//
//
//
//int main() {
//
//	PIP_ADAPTER_INFO pAdapterInfo;
//	PIP_ADAPTER_INFO pAdapter = NULL;
//	DWORD dwRetVal = 0;
//	UINT i;
//
//	/* variables used to print DHCP time info */
//	struct tm newtime;
//	char buffer[32];
//	errno_t error;
//
//	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
//	pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
//	if (pAdapterInfo == NULL) {
//		printf("Error allocating memory needed to call GetAdaptersinfo\n");
//		return 1;
//	}
//	// Make an initial call to GetAdaptersInfo to get
//	// the necessary size into the ulOutBufLen variable
//	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
//		FREE(pAdapterInfo);
//		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
//		if (pAdapterInfo == NULL) {
//			printf("Error allocating memory needed to call GetAdaptersinfo\n");
//			return 1;
//		}
//	}
//
//	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
//		pAdapter = pAdapterInfo;
//
//		while (pAdapter) {
//
//			printf("\tComboIndex: \t%d\n", pAdapter->ComboIndex);
//			printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
//			printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
//			printf("\tAdapter Addr: \t");
//			for (i = 0; i < pAdapter->AddressLength; i++) {
//				if (i == (pAdapter->AddressLength - 1))
//					printf("%.2X\n", (int)pAdapter->Address[i]);
//				else
//					printf("%.2X-", (int)pAdapter->Address[i]);
//			}
//			printf("\tIndex: \t%d\n", pAdapter->Index);
//			printf("\tType: \t");
//			switch (pAdapter->Type) {
//			case MIB_IF_TYPE_OTHER:
//				printf("Other\n");
//				break;
//			case MIB_IF_TYPE_ETHERNET:
//				printf("Ethernet\n");
//				break;
//			case MIB_IF_TYPE_TOKENRING:
//				printf("Token Ring\n");
//				break;
//			case MIB_IF_TYPE_FDDI:
//				printf("FDDI\n");
//				break;
//			case MIB_IF_TYPE_PPP:
//				printf("PPP\n");
//				break;
//			case MIB_IF_TYPE_LOOPBACK:
//				printf("Lookback\n");
//				break;
//			case MIB_IF_TYPE_SLIP:
//				printf("Slip\n");
//				break;
//			default:
//				printf("Unknown type %ld\n", pAdapter->Type);
//				break;
//			}
//
//			printf("\tIP Address: \t%s\n",
//				pAdapter->IpAddressList.IpAddress.String);
//			printf("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);
//
//			printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
//			printf("\t***\n");
//
//			if (pAdapter->DhcpEnabled) {
//				printf("\tDHCP Enabled: Yes\n");
//				printf("\t  DHCP Server: \t%s\n",
//					pAdapter->DhcpServer.IpAddress.String);
//
//				printf("\t  Lease Obtained: ");
//				/* Display local time */
//				error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseObtained);
//				if (error)
//					printf("Invalid Argument to _localtime32_s\n");
//				else {
//					// Convert to an ASCII representation 
//					error = asctime_s(buffer, 32, &newtime);
//					if (error)
//						printf("Invalid Argument to asctime_s\n");
//					else
//						/* asctime_s returns the string terminated by \n\0 */
//						printf("%s", buffer);
//				}
//
//				printf("\t  Lease Expires:  ");
//				error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseExpires);
//				if (error)
//					printf("Invalid Argument to _localtime32_s\n");
//				else {
//					// Convert to an ASCII representation 
//					error = asctime_s(buffer, 32, &newtime);
//					if (error)
//						printf("Invalid Argument to asctime_s\n");
//					else
//						/* asctime_s returns the string terminated by \n\0 */
//						printf("%s", buffer);
//				}
//			}
//			else
//				printf("\tDHCP Enabled: No\n");
//
//			if (pAdapter->HaveWins) {
//				printf("\tHave Wins: Yes\n");
//				printf("\t  Primary Wins Server:    %s\n",
//					pAdapter->PrimaryWinsServer.IpAddress.String);
//				printf("\t  Secondary Wins Server:  %s\n",
//					pAdapter->SecondaryWinsServer.IpAddress.String);
//			}
//			else
//				printf("\tHave Wins: No\n");
//
//			if (strcmp("0.0.0.0", pAdapter->GatewayList.IpAddress.String) != 0) {
//				printf("\t FIND\n");
//				break;
//			}
//			else {
//				printf("\t NOT FIND\n");
//				pAdapter = pAdapter->Next;
//			}
//		}
//	}
//	else {
//		printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
//
//	}
//
//
//	printf("\n\n[CURRENT INFOMATION]\n");
//	printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
//	printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
//	printf("\tAdapter Addr: \t");
//	for (i = 0; i < pAdapter->AddressLength; i++) {
//		if (i == (pAdapter->AddressLength - 1))
//			printf("%.2X\n", (int)pAdapter->Address[i]);
//		else
//			printf("%.2X-", (int)pAdapter->Address[i]);
//	}
//	printf("\tIP Address: \t%s\n", pAdapter->IpAddressList.IpAddress.String);
//	printf("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);
//	printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
//
//	if (pAdapterInfo)
//		FREE(pAdapterInfo);
//
//	return 0;
//}