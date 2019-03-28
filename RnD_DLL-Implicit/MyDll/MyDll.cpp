#define ver 1

/*
	�Ͻ���(implicit) ���� 
	- �Լ��� ��� DLL�� �ִ��� ������ ����
	- Ŭ���̾�Ʈ ������Ʈ�� DLL������Ʈ���� ����Ǿ ���� lib�� dll�� ������ ���Խ������ �Ѵ� or #pragma comment(lib,"MyDll.lib")�� �߰����ش�
	- ����� ����(Load time Linking)�ȴ�.

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




