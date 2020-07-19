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

	//for (int i = 0; i < 16; i++)
	//{
	//	key[i] = *(const_cast<char*>(str_seed_key.c_str() + i));
	//	iv[i] = *(const_cast<char*>(str_seed_key.c_str() + i));
	//}

	/* ��ĪŰ�� IV */
	//unsigned char key[16] = { 0xED, 0x24, 0x01, 0xAD, 0x22, 0xFA, 0x25, 0x59, 0x91, 0xBA, 0xFD, 0xB0, 0x1F, 0xEF, 0xD6, 0x97 };
	//unsigned char iv[16] = { 0x93, 0xEB, 0x14, 0x9F, 0x92, 0xC9, 0x90, 0x5B, 0xAE, 0x5C, 0xD3, 0x4D, 0xA0, 0x6C, 0x3C, 0x8E };
	// chlguswls!@#giga
	unsigned char key[16] = { 0x63, 0x68, 0x6c, 0x67, 0x75, 0x73, 0x77, 0x6C, 0x73, 0x21, 0x40, 0x23, 0x67, 0x69, 0x67, 0x61 };
	unsigned char iv[16] = { 0x63, 0x68, 0x6c, 0x67, 0x75, 0x73, 0x77, 0x6C, 0x73, 0x21, 0x40, 0x23, 0x67, 0x69, 0x67, 0x61 };

	//unsigned char key[16] = { 'A','2','3','1','9','3','A','8','0','F','1','5','M','S','0','7' };
	//unsigned char iv[16] = { 'D','C','8','8','E','0','F','5','1','B','6','0','0','0','1','6' };

	//for (int i = 0; i < 16; i++)
	//{
	//	key[i] = *(const_cast<char*>(str_seed_key.c_str() + i));
	//	iv[i] = *(const_cast<char*>(str_seed_key.c_str() + i));
	//}

	
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
	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale());

	const unsigned char* test_str = (const unsigned char*)"�ѱ�123abc";
	int test_size = 0;
	unsigned char* test_out_str;
	test_out_str = __base64_encode(test_str, strlen((char*)test_str), &test_size);

	std::cout << test_out_str << std::endl;

	unsigned char* test_out_decode_str;
	int test_out_size = strlen((char*)test_out_str);

	test_out_decode_str = __base64_decode(test_out_str, strlen((char*)test_out_str), &test_out_size);
	std::cout << test_out_decode_str << std::endl;

	std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
	std::cout << "��ȣȭ �׽�Ʈ" << std::endl;

	char* plainStr = NULL;
	int plainStr_Size = 0;

	unsigned char seed_encoded_Str[BUF_SIZE + 16] = "\0";
	int encoded_Cyper_Size = 0;

	unsigned char* base64_encoded_Str = NULL;
	int base64_encoded_Size = 0;

	plainStr = (char*)"{\r\n  \"set-tx-configuration\": {\r\n    \"tx-mac\": \"00:67:a8:2d:84:7e\",\r\n    \"tx-ip\": \"232.123.132.154\",\r\n    \"rx-said\": \"PC000001\",\r\n    \"rx-client-id\": \"00:52:a8:2d:84:4e\",\r\n    \"rx-cust-id\":\"\",\r\n    \"auth-method\":\"auth-pw\",\r\n    \"port-1\": \"10001\",\r\n    \"port-2\": \"10002\",\r\n    \"port-3\": \"10003\",\r\n    \"port-4\": \"10004\",\r\n    \"port-5\": \"10005\",\r\n    \"zone-id\":\"ZONE0001\",\r\n    \"option\":\" \"\r\n  }\r\n}\r\n";
	plainStr_Size = strlen((char*)plainStr);

	// SEED Encode
	encoded_Cyper_Size = KISA_SEED_CBC_ENCRYPT(key, iv, (unsigned char*)plainStr, plainStr_Size, seed_encoded_Str);

	// Base64 Encode
	//! You must "free()" output str after process done
	base64_encoded_Str = __base64_encode((unsigned char*)seed_encoded_Str, encoded_Cyper_Size, &base64_encoded_Size);


	std::cout << "[Encode] PlainStr : " << base64_encoded_Str << std::endl;
	std::cout << "[Encode] PlainStr size : " << base64_encoded_Size << std::endl;
	std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;



	std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
	std::cout << "��ȣȭ �׽�Ʈ" << std::endl;

	unsigned char* base64_CyperStr = NULL;
	int base64_CyperStr_Size = 0;
	
	unsigned char* seed_CyperStr = NULL;

	unsigned char decoded_PlainStr[BUF_SIZE] = "\0";
	int decoded_PlainStr_Size = 0;


	// Init base64 cyper String
	base64_CyperStr = (unsigned char*)"pBvdqQGRAsecaAT4V2kMvw==";
	base64_CyperStr_Size = strlen((char*)base64_CyperStr); // Not include \0
	//base64_CyperStr = base64_encoded_Str;
	//base64_CyperStr_Size = strlen((char*)base64_CyperStr); // Not include \0

	// Base64 Decode
	//! You must "free()" output str after process done
	seed_CyperStr = __base64_decode(base64_CyperStr, strlen((char*)base64_CyperStr), &base64_CyperStr_Size);

	std::cout << "base64 decode after size : " << base64_CyperStr_Size << std::endl;

	// SEED Decode
	decoded_PlainStr_Size = KISA_SEED_CBC_DECRYPT(key, iv, seed_CyperStr, base64_CyperStr_Size, decoded_PlainStr);

	std::cout << "[Decode] PlainStr : " << decoded_PlainStr << std::endl;
	std::cout << "[Decode] PlainStr size : " << decoded_PlainStr_Size << std::endl;
	std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;

	free(base64_encoded_Str);
	free(seed_CyperStr);



	while(1){
		memset(&recv_text, 0, sizeof(recv_text));
		
		/* ��ȣȭ ���� */
		//printf("SEED ��ȣȭ�� �����͸� �Է��ϼ���: ");
		//fgets((char *)plaintext, sizeof(plaintext), stdin); /* ��ȣȭ�� �� ������Է� */

		//char* plaintext = (char*)"{\r\n  \"hello\": {\r\n    \"client-info\": {\r\n      \"said\":\"MSLM0001\",\r\n      \"client-id\":\"MSLM0001\",\r\n      \"device-name\": \"SW RX\",\r\n      \"device-version\": \"1.0.0\",\r\n      \"platform\": \"syncpc-client-OS\",\r\n      \"locale\": \"ko_KO\",\r\n      \"hostname\": \"syncpc.com\",\r\n      \"serial-number\": \"123456\",\r\n      \"mac\": \"00:67:a8:2d:84:7e\"\r\n     },\r\n    \"CBS-info\": {\r\n      \"device-name\": \"kt Connection Broker\",\r\n      \"device-version\": \"1.0.0.12345\",\r\n      \"platform\": \"Linux Ubuntu 10.4 64-bit\",\r\n      \"ip-address\": \"10.0.136.93\",\r\n      \"hostname\": \"CBS.kt.com\"\r\n     },\r\n    \"negotiable capability\": {\r\n      \"capability\": [\r\n        \"NEGO_OAUTH\",\r\n        \"NEGO_PASSWORD\"\r\n      ]\r\n    },\r\n    \"client-RMS-options\": {\r\n      \"option\": {\r\n        \"name\": \"option-name\",\r\n        \"value\": \"option-value\"\r\n      }\r\n    }\r\n  }\r\n}";
		char* plaintext = (char*)"{\r\n  \"set-tx-configuration\": {\r\n    \"tx-mac\": \"00:67:a8:2d:84:7e\",\r\n    \"tx-ip\": \"232.123.132.154\",\r\n    \"rx-said\": \"PC000001\",\r\n    \"rx-client-id\": \"00:52:a8:2d:84:4e\",\r\n    \"rx-cust-id\":\"\",\r\n    \"auth-method\":\"auth-pw\",\r\n    \"port-1\": \"10001\",\r\n    \"port-2\": \"10002\",\r\n    \"port-3\": \"10003\",\r\n    \"port-4\": \"10004\",\r\n    \"port-5\": \"10005\",\r\n    \"zone-id\":\"ZONE0001\",\r\n    \"option\":\" \"\r\n  }\r\n}\r\n";
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
		//printf("%s\n", str);

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

		str = (unsigned char*)"7hnLAjhTj+zd+sjhAwN/5Q==";
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


std::string ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = 0; //stringó������ �˻�
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from�� ã�� �� ���� ������
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // �ߺ��˻縦 ���ϰ� from.length() > to.length()�� ��츦 ���ؼ�
	}
	return str;
}