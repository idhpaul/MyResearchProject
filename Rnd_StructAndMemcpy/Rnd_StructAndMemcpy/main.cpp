#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct data {
	int head;
	int* iptr1;
	int* iptr2;
	int* iptr3;
}teststurct;

void copy_data(teststurct* ts_main) {
	teststurct ts_infunc;
	memset(&ts_infunc, 0, sizeof(teststurct));
	int i1 = 1;
	int i2 = 2;
	int i3 = 3;

	ts_infunc.head = 1;
	ts_infunc.iptr1 = &i1;
	ts_infunc.iptr2 = &i2;
	ts_infunc.iptr3 = &i3;

	memcpy(ts_main, &ts_infunc, sizeof(teststurct));
}

int main() {

	teststurct ts_main;
	memset(&ts_main, 0, sizeof(teststurct));

	copy_data(&ts_main);

	printf("end\n");

	return 0;
}