#define _CRT_SECURE_NO_WARNINGS

// !wctomb 예제
//#include <stdio.h>
//#include <stdlib.h>
//#include <wchar.h>
//#include <locale.h>
//
//int main() {
//
//	_wsetlocale(LC_ALL, L"korean");
//
//	const wchar_t *tstr = L"ABC 가나다 123";
//	const char *charstr = "ABC 가나다 123";
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

// !wctombs 문자열 변환 _ 버퍼가 필요함
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
//	setlocale(LC_ALL, "Korean");//로케일 설정
//	last = wcstombs(str, wstr, sizeof(str));//유니코드 스트링을 멀티바이트 스트링으로 변환
//	str[last] = 0;//종료 문자열 설정
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

	printf("%d 숫자, %s 일반 문자열, %s 와이드캐릭터", 1, "일반문자열입니다.", L"2017-16-16 15:15:15");

	//결과 와이드캐릭터 출력이 안됨

	return -1;
}