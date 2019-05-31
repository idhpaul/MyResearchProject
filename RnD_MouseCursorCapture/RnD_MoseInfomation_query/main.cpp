#include <stdio.h>
#include <Windows.h>

int main(void) {

	while (true) {

		CURSORINFO Info = { 0 };
		Info.cbSize = sizeof(Info);
		GetCursorInfo(&Info);

		if (Info.hCursor == (HCURSOR)0x0000000000010005)
			printf("일반\n");
		else if (Info.hCursor == (HCURSOR)0x000000000001001b)
			printf("실행\n");
		else if (Info.hCursor == (HCURSOR)0x0000000000010007)
			printf("I Beam\n");
		else if (Info.hCursor == (HCURSOR)0x0000000000010021)
			printf("선택 손\n");
		else if (Info.hCursor == (HCURSOR)0x0000000000010015)
			printf("수직 화살표\n");
		else if (Info.hCursor == (HCURSOR)0x0000000000010013)
			printf("수평 화살표\n");
		else if (Info.hCursor == (HCURSOR)0x000000000001000f)
			printf("대각선(4시10시) 화살표\n");
		else if (Info.hCursor == (HCURSOR)0x0000000000010011)
			printf("대각선(2시7시) 화살표\n");
		Sleep(500);
	}

	return 0;
}