#include <Windows.h>
#include <stdio.h>


int main(void) {

	HCURSOR arrow_default = LoadCursor(0, IDC_ARROW);
	HCURSOR arrow_ibeam = LoadCursor(0, IDC_IBEAM);
	HCURSOR arrow_nwse = LoadCursor(0, IDC_SIZENWSE);
	HCURSOR arrow_nesw = LoadCursor(0, IDC_SIZENESW);
	HCURSOR arrow_we = LoadCursor(0, IDC_SIZEWE);
	HCURSOR arrow_ns = LoadCursor(0, IDC_SIZENS);
	HCURSOR arrow_running = LoadCursor(0, IDC_APPSTARTING);
	HCURSOR arrow_link = LoadCursor(0, IDC_HAND);
	HCURSOR arrow_cross = LoadCursor(0, IDC_CROSS);
	HCURSOR arrow_help = LoadCursor(0, IDC_HELP);
	HCURSOR arrow_no = LoadCursor(0, IDC_NO);
	HCURSOR arrow_all = LoadCursor(0, IDC_SIZEALL);
	HCURSOR arrow_wait = LoadCursor(0, IDC_WAIT);

	while (true) {

		CURSORINFO Info = { 0 };
		Info.cbSize = sizeof(Info);
		GetCursorInfo(&Info);

		if (Info.hCursor == arrow_default) {
			//shape:�Ϲ�
			printf("�Ϲ�Ŀ�� �ڵ尪:%x\n", Info.hCursor);
		}
		else if (Info.hCursor == arrow_ibeam) {
			//shape:ibeam
			printf("iBeamĿ�� �ڵ尪:%x\n", Info.hCursor);
		}
		else if (Info.hCursor == arrow_nwse) {
			//shape:��
			printf("��Ŀ�� �ڵ尪:%x\n", Info.hCursor);
		}
		else if (Info.hCursor == arrow_nesw) {
			//shape:��
			printf("��Ŀ�� �ڵ尪:%x\n", Info.hCursor);
		}
		else if (Info.hCursor == arrow_we) {
			//shape:��
			printf("��Ŀ�� �ڵ尪:%x\n", Info.hCursor);
		}
		else if (Info.hCursor == arrow_ns) {
			//shape:��
			printf("��Ŀ�� �ڵ尪:%x\n", Info.hCursor);
		}
		else if (Info.hCursor == arrow_running) {
			printf("����Ŀ�� �ڵ尪:%x\n", Info.hCursor);
		}
		else if (Info.hCursor == arrow_link) {
			printf("��Ŀ�� �ڵ尪:%x\n", Info.hCursor);
		}
		else if (Info.hCursor == arrow_wait) {
			printf("�𷡽ð� �ڵ尪:%x\n", Info.hCursor);
		}
		else {
			printf("���� ���� ���콺 �ڵ尪:%x\n", Info.hCursor);
		}

		Sleep(1000);

	}

	return 0;
}