#include "MyUDPSocket.h"

#include <iostream>

int main()
{
    WSADATA wsaData;
    int iResult = 0;
    //-----------------------------------------------
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        wprintf(L"WSAStartup failed with error %d\n", iResult);
        return 1;
    }

    MyUDPSocket server_udp(udp_client);
    iResult = server_udp.Handshake("127.0.0.1",8090);


    //-----------------------------------------------
    // Clean up and exit.
    wprintf(L"Exiting.\n");
    WSACleanup();

	return 0;
}
