#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <locale>
#include <sstream>
using namespace std;
#pragma comment(lib,"ws2_32.lib")


string website_HTML;
locale local;
void get_Website(string url);
char lineBuffer[200][80] = { ' ' };
char buffer[10000];
char ip_address[16];
int i = 0, bufLen = 0, j = 0, lineCount = 0;
int lineIndex = 0, posIndex = 0;

//****************************************************

int main(void) {
	cout << "\n\n\n";
	get_Website("api.ipify.org");
	for (size_t i = 0; i < website_HTML.length(); ++i) website_HTML[i] = tolower(website_HTML[i], local);

	istringstream ss(website_HTML);
	string stoken;

	while (getline(ss, stoken, '\n')) {

		//cout <<"-->"<< stoken.c_str() << '\n';

		strcpy_s(lineBuffer[lineIndex], stoken.c_str());
		int dot = 0;
		for (int ii = 0; ii < strlen(lineBuffer[lineIndex]); ii++) {

			if (lineBuffer[lineIndex][ii] == '.') dot++;
			if (dot >= 3) {
				dot = 0;
				strcpy_s(ip_address, lineBuffer[lineIndex]);
			}
		}

		lineIndex++;
	}
	cout << "Your IP Address is  " << ip_address << " \n\n";


	cout << "\nPress ANY key to close.\n\n";
	cin.ignore(); cin.get();

	return 0;
}

//****************************************************

void get_Website(string url) {
	WSADATA wsaData;
	SOCKET Socket;
	SOCKADDR_IN SockAddr;
	int lineCount = 0;
	int rowCount = 0;
	struct hostent *host;
	string get_http;


	get_http = "GET / HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "WSAStartup failed.\n";
		system("pause");
		//return 1;
	}

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	host = gethostbyname(url.c_str());

	SockAddr.sin_port = htons(80);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0) {
		cout << "Could not connect";
		system("pause");
		//return 1;
	}
	send(Socket, get_http.c_str(), strlen(get_http.c_str()), 0);

	int nDataLength;
	while ((nDataLength = recv(Socket, buffer, 10000, 0)) > 0) {
		int i = 0;
		while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {

			website_HTML += buffer[i];
			i += 1;
		}
	}

	closesocket(Socket);
	WSACleanup();

}