#include "overlap_test.h"
#include "overlap_test2.h"

#include <stdio.h>

int main() {

	printf("1. 하나의 .cpp에 하나의 .obj가 나온다.");
	printf("2. 헤더의 중복 정의를 막기 위해서는 #ifndef 전처리기를 이용한다.");
	printf("3. 헤더파일 생성시 #ifndef 구조를 만드는 습관을 들이자");

// ! ifndef 구조
//#ifndef __OVERLAP_HELLO_H
//#define __OVERLAP_HELLO_H
//
//
	//헤더내용
//
//
//#endif //__OVERLAP_HELLO_H

	return 0;
}