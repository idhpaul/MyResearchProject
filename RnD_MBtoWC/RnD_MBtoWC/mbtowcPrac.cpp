#define _CRT_SECURE_NO_WARNINGS

// !wctomb ����
//#include <stdio.h>
//#include <stdlib.h>
//#include <wchar.h>
//#include <locale.h>
//
//int main() {
//
//	_wsetlocale(LC_ALL, L"korean");
//
//	const wchar_t *tstr = L"ABC ������ 123";
//	const char *charstr = "ABC ������ 123";
//
//	const wchar_t str[] = L"1123123123123123123123123123";
//	const wchar_t* pt;
//	char buffer[256];
//	int i, length;
//
//	pt = str;
//	while (*pt) {
//		length = wctomb(buffer, *pt);
//		if (length < 1)
//			break;
//		for (i = 0; i < length; ++i)
//			printf("[%c]", buffer[i]);
//		++pt;
//	}
//
//	wprintf(L"%s\n", tstr);
//	printf("%s\n", charstr);
//
//
//	return 0;
//}

// !wctombs ���ڿ� ��ȯ _ ���۰� �ʿ���
//#include <locale.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdio.h>
//
//int main(void)
//{
//	wchar_t wstr[] = L"1";
//	char str[100] = { 0, };
//	int last;
//
//	setlocale(LC_ALL, "Korean");//������ ����
//	last = wcstombs(str, wstr, sizeof(str));//�����ڵ� ��Ʈ���� ��Ƽ����Ʈ ��Ʈ������ ��ȯ
//	str[last] = 0;//���� ���ڿ� ����
//	printf("%s\n", str);
//
//	int changeint=0;
//	changeint = atoi(str);
//	printf("%d\n", changeint);
//
//
//	return 0;
//}

#include <stdio.h>
#include <wchar.h>

int main() {

	printf("%d ����, %s �Ϲ� ���ڿ�, %s ���̵�ĳ����", 1, "�Ϲݹ��ڿ��Դϴ�.", L"2017-16-16 15:15:15");

	//��� ���̵�ĳ���� ����� �ȵ�

	return -1;
}