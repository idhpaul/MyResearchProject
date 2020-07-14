#define _CRT_SECURE_NO_WARNINGS

#define BUF_SIZE 10240
#define SOCKET_TEST_SIM 0

#if SOCKET_TEST_SIM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Winsock2.h>

#define SERVER_IP "172.16.10.141"
#define PORT_NUM 5005
#endif

#include "b64.h"
#include "seedcbc.h"
#include <atlconv.h>

#include <string>
#include <iostream>


int print_decryptdata(int after_decrypt_len, unsigned char after_decrypt_plaintext[], unsigned char *dst, int size);
void error_handling(char *msg);

void convert_string2wstring(const std::string& src, std::wstring& dest);
void convert_wstring2string(const std::wstring& src, std::string& dest);
std::string ReplaceAll(std::string& str, const std::string& from, const std::string& to);

int main(){

	std::string str_seed_key = "chlguswls!@#giga";
	
	std::cout << str_seed_key.length() << std::endl;
	if (16 != str_seed_key.length())
	{
		std::cout << "Seed Ű�� ����(16byte)�� �������� �ʾҽ��ϴ�." << std::endl;
		return -1;
	}

	/* ��ĪŰ�� IV */
	//unsigned char key[16] = { 0xED, 0x24, 0x01, 0xAD, 0x22, 0xFA, 0x25, 0x59, 0x91, 0xBA, 0xFD, 0xB0, 0x1F, 0xEF, 0xD6, 0x97 };
	//unsigned char iv[16] = { 0x93, 0xEB, 0x14, 0x9F, 0x92, 0xC9, 0x90, 0x5B, 0xAE, 0x5C, 0xD3, 0x4D, 0xA0, 0x6C, 0x3C, 0x8E };
	// chlguswls!@#giga
	//unsigned char key[16] = { 0x63, 0x68, 0x6c, 0x67, 0x75, 0x73, 0x77, 0x6C, 0x73, 0x21, 0x40, 0x23, 0x67, 0x69, 0x67, 0x61 };
	//unsigned char iv[16] = { 0x63, 0x68, 0x6c, 0x67, 0x75, 0x73, 0x77, 0x6C, 0x73, 0x21, 0x40, 0x23, 0x67, 0x69, 0x67, 0x61 };

	unsigned char key[16] = {0,};
	unsigned char iv[16] = {0,};

	for (int i = 0; i < 16; i++)
	{
		key[i] = *(const_cast<char*>(str_seed_key.c_str() + i));
		iv[i] = *(const_cast<char*>(str_seed_key.c_str() + i));
	}

	
	/* ����� ���� */
	unsigned char plaintext[BUF_SIZE] = "\0";
	unsigned char ciphertext[BUF_SIZE+16] = "\0";

	unsigned char after_decrypt_plaintext[BUF_SIZE] = "\0"; /* ��ȣȭ�� ���� ����¹��� */
	unsigned char recv_text[BUF_SIZE] = "\0";

	unsigned char *str = NULL; /* str : base64 ���ڵ� ���� ������ */
	unsigned char *dst = NULL; /* dst : base64 ���ڵ� ���� ������ */
	
	int  size = 0;
	int recv_text_length = 0;
	int plaintext_len = 0; /* �Է¹��� plaintext�� ���� */
	int after_decrypt_len = 0; /* ��ȣȭ �� ������ ���� */
	int after_encrypt_len = 0; /* ��ȣȭ �� ������ ���� */
	
#if SOCKET_TEST_SIM
	/*
	* ���� ���̺귯�� �ʱ�ȭ
	* 2.2������ winsock���
	*/

	WSADATA wsa_data;
	SOCKET connect_sock;
	SOCKADDR_IN connect_addr;

	if(WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0){
		error_handling("WSAStartup() Error");
	}
	printf("socket ���̺귯�� �ʱ�ȭ\n");

	/* ������ ������ SOCKET ����ü�� ����*/
	connect_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(connect_sock == INVALID_SOCKET){
		error_handling("socket() Error");
	}
	printf("socket ����\n");

	memset(&connect_addr, 0, sizeof(connect_addr));
	connect_addr.sin_family = AF_INET; /* Internet Protocol Version(IPv4) */
	connect_addr.sin_addr.s_addr = inet_addr(SERVER_IP); /* IP�ּ� ���� */
	connect_addr.sin_port = htons(PORT_NUM); /* port��ȣ ���� */
	
	printf("������...\n");
	/* ����ü�� ����� �ּҷ� connect_sock ������ ���� ���� �õ� */
	if(connect(connect_sock, (SOCKADDR*)&connect_addr, sizeof(connect_addr)) == SOCKET_ERROR){
		printf("Err_No: %d\n", WSAGetLastError());
		error_handling("connect() Error");
	}
	printf("���Ἲ��\n");

#endif

	while(1){
		memset(&recv_text, 0, sizeof(recv_text));
		
		/* ��ȣȭ ���� */
		//printf("SEED ��ȣȭ�� �����͸� �Է��ϼ���: ");
		//fgets((char *)plaintext, sizeof(plaintext), stdin); /* ��ȣȭ�� �� ������Է� */

		char* plaintext = (char*)"{\r\n  \"hello\": {\r\n    \"client-info\": {\r\n      \"said\":\"MSLM0001\",\r\n      \"client-id\":\"MSLM0001\",\r\n      \"device-name\": \"SW RX\",\r\n      \"device-version\": \"1.0.0\",\r\n      \"platform\": \"syncpc-client-OS\",\r\n      \"locale\": \"ko_KO\",\r\n      \"hostname\": \"syncpc.com\",\r\n      \"serial-number\": \"123456\",\r\n      \"mac\": \"00:67:a8:2d:84:7e\"\r\n     },\r\n    \"CBS-info\": {\r\n      \"device-name\": \"kt Connection Broker\",\r\n      \"device-version\": \"1.0.0.12345\",\r\n      \"platform\": \"Linux Ubuntu 10.4 64-bit\",\r\n      \"ip-address\": \"10.0.136.93\",\r\n      \"hostname\": \"CBS.kt.com\"\r\n     },\r\n    \"negotiable capability\": {\r\n      \"capability\": [\r\n        \"NEGO_OAUTH\",\r\n        \"NEGO_PASSWORD\"\r\n      ]\r\n    },\r\n    \"client-RMS-options\": {\r\n      \"option\": {\r\n        \"name\": \"option-name\",\r\n        \"value\": \"option-value\"\r\n      }\r\n    }\r\n  }\r\n}";

		//std::string strtest = plaintext;
		//ReplaceAll(strtest, std::string("\""), std::string("@"));
		//std::cout << strtest << std::endl;

		plaintext_len = strlen((char *)plaintext); /* �Է¹��� ���� ���� ��� */
		
		/* �Է¹��� �����Ͱ� exit�� ���� */
		if(strcmp((char *)plaintext, "exit\n") == 0){
			//send(connect_sock, (char *)plaintext, strlen((char *)plaintext), 0);
			break;
		}

		printf("plaint text : %s\n", plaintext);

		/* SEED-CBC ��ȣȭ */
		after_encrypt_len = KISA_SEED_CBC_ENCRYPT(key, iv, (unsigned char*)plaintext, plaintext_len, ciphertext);

		/* ��ȣȭ�� �����͸� Base64 ���ڵ� */
		str = __base64_encode((unsigned char *)ciphertext, after_encrypt_len, &size);

		printf("base64 enc\n");
		printf("%s\n", str);

#if SOCKET_TEST_SIM
		/* ������ ��ȣȭ�� ������ ���� */
		send(connect_sock, (const char *)str, size, 0);
		printf("��ȣȭ ������ ���ۼ���\n");*/

		/* �����κ��� ���۵Ǵ� ������ ���� */
		recv_text_length = recv(connect_sock, (char *)recv_text, sizeof(recv_text) - 1, 0);
		if(recv_text_length == -1){
			error_handling("read() error");
		}
		printf("�����κ��� ���� ������: %s", recv_text);
#endif

		str = (unsigned char*)"Zt6UXN8g7RSAlyScgc4i1DsiFQMCGsHhJEN0zviwCyAbBgoMHRMsqHQLtnCboHpIe4D9hSiank2WegX6TERT6/KI7WDqUyvAjE+QtBqD1my9nhQHrxJIRz3ls6qqCR8eK4RaY5S9R06SpzohyaB9oPKDi9gOdb5Vl8pOIBhGRU+IFpNgApIxg+o+h82ZrNVZ25nvVbrG6KVHttotdEbWEZI++U/sZ103TlBttlz+WSe3VBM7xAn4dAKHgFTsA+Rz8QpmuqFWA1NrFnV7eJBWauJook127LT1qCu8bLQSIbSd7lLDKMDtH85Y04y41lxUoPf4hkflgDhMvE9Z9VPvVlTpcxwzBKPaYyFXkuB5NmZnAts2lumYP40UcVMP7hWfuSn/nSCoXNjGl0elPCcwgl8DW1Tzw4pLWclwLQNmJxhZ9jLRYNFV+xUHItC5By5qgi62co2SyJDmNe0wcVYSHRmT8rS1W6nc4QFgl8j0nGBaOtAPQJ3fmU0qZZwZ/hs1HmIGvkYjYNxAVa6/k0UGFzmqlc7EuWMVCJSahKuwZLdIjRzL2Wrdk7Cy3/Dau429tKwUOmvcE1IxTvdIwPihZGoFRhUo9AJzX8SmEoVx/9NNsGLexLRvS13+ugQVVJOM5cEWIG/0X3Bz0RuNpE+CH3YRNKGKVkSwMsfEpNQ0Hwd6nOMmEVIfTIu31VBSAiYLsl0ycDeZWCPDSNvsKwZZONySbFX2C90lGFKEhOqIQXaOVh0GgmKzVE//kwld6hIV36H6PRkTdtZ1F3Cs/p+wZQ==";
		size = strlen((char*)str);

		/* �����κ��� ���� ��ȣȭ �����͸� Base64 ���ڵ� */
		//dst = __base64_decode(recv_text, strlen((char*)recv_text), &size);
		dst = __base64_decode(str, strlen((char *)str), &size);

		/* SEED-CBC ��ȣȭ */
		after_decrypt_len = KISA_SEED_CBC_DECRYPT(key, iv, dst, size, after_decrypt_plaintext);

		printf("base64 -> seed dec %s\n", after_decrypt_plaintext);


		/* ��ȣȭ ������ ��� */
		print_decryptdata(after_decrypt_len, after_decrypt_plaintext, dst, size);
		
		// free error  �� üũ �ٶ�
		//free(str);
		//free(dst);

		break;
		
		printf("--------------------------------------------------------------\n");
	}
	printf("��������\n");
	
#if SOCKET_TEST_SIM

	closesocket(connect_sock); /* ���� �ݱ� */
	WSACleanup();/* winsock ���� */

#endif

	return 0;
}

int print_decryptdata(int after_decrypt_len, unsigned char after_decrypt_plaintext[], unsigned char *dst, int size){
	int i = 0;

	//printf("\n���ڵ� �� ������: %s\n\n���ڵ� �� ������ ����: %d\n", dst, size);

	printf("\n������ ��ȣȭ ���: ");
	for (i = 0; i < after_decrypt_len; i++)
		printf("%c", after_decrypt_plaintext[i]);
	//printf("\n��ȣȭ �� ������ ����: %d\n", plainlen);
	printf("\n");

	return 0;
}

void error_handling(char *msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void convert_string2wstring(const std::string& src, std::wstring& dest)
{
	USES_CONVERSION;
	dest = A2W(src.c_str());
}

void convert_wstring2string(const std::wstring& src, std::string& dest)
{
	USES_CONVERSION;
	dest = W2A(src.c_str());
}

std::string ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = 0; //stringó������ �˻�
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from�� ã�� �� ���� ������
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // �ߺ��˻縦 ���ϰ� from.length() > to.length()�� ��츦 ���ؼ�
	}
	return str;
}