//#define _CRT_SECURE_NO_WARNINGS
//#include <stdio.h>
//#include <wchar.h>
//
//int main()
//{
//	wchar_t ws1[20];
//	wscanf(L"%s", ws1);
//	wprintf(L"%s\n", ws1);
//
//	// �� ��° �μ����� wchar_t ���ڿ��� ���� ������ ���ؼ� �־���
//	swprintf(ws1, sizeof(ws1) / sizeof(wchar_t), L"%s", L"Hello, world!");
//
//	FILE *fp = _wfopen(L"hello.txt", L"w+");
//	// _wfopen �Լ��� ǥ���� �ƴϹǷ� ������������ ����� �� ����
//	// ������������ fopen �Լ� ���
//	// FILE *fp = fopen("hello.txt", "w+");
//
//	// ������������ %s ��� %S ���
//	wchar_t ws2[20];
//	wchar_t ws3[20];
//	fwprintf(fp, L"%s", L"C Language");
//	rewind(fp);
//	fwscanf(fp, L"%s %s", ws2, ws3);
//	wprintf(L"%s %s\n", ws2, ws3);
//
//	rewind(fp);
//	fputws(L"Wide Character", fp);
//	rewind(fp);
//	fgetws(ws2, 20, fp);
//	wprintf(L"%s\n", ws2);
//
//	rewind(fp);
//	fputwc(L'X', fp);
//	rewind(fp);
//	wint_t wc1 = fgetwc(fp);
//	putwchar(wc1);
//
//	fclose(fp);
//
//	return 0;
//}