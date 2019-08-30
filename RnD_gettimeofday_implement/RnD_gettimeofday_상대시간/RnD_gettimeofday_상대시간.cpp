// RnD_gettimeofday_상대시간.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//


#include "pch.h"

#include <Windows.h>

#include <iostream>
#include <ctime>

int gettimeofday(struct timeval* tp, int* tz)
{
	LARGE_INTEGER tickNow;
	static LARGE_INTEGER tickFrequency;
	static BOOL tickFrequencySet = FALSE;
	if (tickFrequencySet == FALSE) {
		QueryPerformanceFrequency(&tickFrequency);
		tickFrequencySet = TRUE;
	}
	QueryPerformanceCounter(&tickNow);
	tp->tv_sec = (long)(tickNow.QuadPart / tickFrequency.QuadPart);
	tp->tv_usec = (long)(((tickNow.QuadPart % tickFrequency.QuadPart) * 1000000L) / tickFrequency.QuadPart);

	return 0;
}

int main()
{
	struct timeval mytime;

	// 현재 시간을 얻어온다.
	// 현재 시간이 1999125초190845마이크로초 라면
	// mytime.tv_sec = 1999125
	// mytime.tv_usec = 190845
	gettimeofday(&mytime, NULL);
	printf("%ld:%ld\n", mytime.tv_sec, mytime.tv_usec);
}

