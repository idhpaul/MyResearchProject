// RnD_gettimeofday_절대시간(unix).cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <stdio.h>
#include <time.h>
#include <windows.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone
{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};

typedef int int64_t;
typedef unsigned int uint64_t;

int GetTimeOfDay(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	uint64_t tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		// system time을 구하기
		GetSystemTimeAsFileTime(&ft);

		// unsigned 64 bit로 만들기
		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		// 100nano를 1micro로 변환하기
		tmpres /= 10;

		// epoch time으로 변환하기
		tmpres -= DELTA_EPOCH_IN_MICROSECS;

		// sec와 micorsec으로 맞추기
		tv->tv_sec = (tmpres / 1000000UL);
		tv->tv_usec = (tmpres % 1000000UL);
	}

	// timezone 처리
	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}

int64_t GetTimeOfDay()
{
	struct timeval now;
	GetTimeOfDay(&now, NULL);

	int64_t millisecond = (int64_t)(now.tv_sec) * 1000 + (int64_t)(now.tv_usec) / 1000;

	return millisecond;
}

int main()
{
	printf("%d\n", GetTimeOfDay());
}

