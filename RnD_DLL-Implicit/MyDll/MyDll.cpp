#define ver 1

/*
	암시적(implicit) 연결 
	- 함수가 어느 DLL에 있는지 밝히지 않음
	- 클라이언트 프로젝트에 DLL프로젝트에서 빌드되어서 나온 lib과 dll의 정보를 포함시켜줘야 한다 or #pragma comment(lib,"MyDll.lib")을 추가해준다
	- 실행시 연결(Load time Linking)된다.

*/

#if ver

extern "C" __declspec(dllexport) int AddInteger(int a, int b)
{
	return a + b;
}

#else

#define DLLEXPORT
#include "MyDll.h"

extern "C" MYDLLTYPE int AddInteger(int a, int b)
{
	return a + b;
}

#endif




