#include "vld.h"

#include <crtdbg.h>
#include <iostream>

using namespace std;

#ifdef _DEBUG
#define new new(_CLIENT_BLOCK,__FILE__,__LINE__)
#define malloc(s) _mallock_dbg(s,_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


int* aNew()
{
	auto p = new int{ 9 };
	return p;
}
int* bNew()
{
	auto p = new int{ 1 };
	return p;
}
void pDelete(int *p)
{
	delete p;
}

int main()
{

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	auto ptr = aNew();
	ptr = bNew();
	pDelete(ptr);

	//종료전 아래 함수를 추가
	//_CrtDumpMemoryLeaks();

	return 0;
}