#include <stdio.h>
#include <Windows.h>

int main(void) {

	while (true) {

		CURSORINFO Info = { 0 };
		Info.cbSize = sizeof(Info);
		GetCursorInfo(&Info);

		if (Info.hCursor == (HCURSOR)0x0000000000010005)
			printf("�Ϲ�\n");
		else if (Info.hCursor == (HCURSOR)0x000000000001001b)
			printf("����\n");
		else if (Info.hCursor == (HCURSOR)0x0000000000010007)
			printf("I Beam\n");
		else if (Info.hCursor == (HCURSOR)0x0000000000010021)
			printf("���� ��\n");
		else if (Info.hCursor == (HCURSOR)0x0000000000010015)
			printf("���� ȭ��ǥ\n");
		else if (Info.hCursor == (HCURSOR)0x0000000000010013)
			printf("���� ȭ��ǥ\n");
		else if (Info.hCursor == (HCURSOR)0x000000000001000f)
			printf("�밢��(4��10��) ȭ��ǥ\n");
		else if (Info.hCursor == (HCURSOR)0x0000000000010011)
			printf("�밢��(2��7��) ȭ��ǥ\n");
		Sleep(500);
	}

	return 0;
}