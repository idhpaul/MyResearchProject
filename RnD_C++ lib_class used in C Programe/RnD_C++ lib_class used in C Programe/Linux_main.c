

#include "MyWrapper.h"
#include <stdio.h>

int main(int argc, char* argv[]) {

	FILE *ptr;

	char fbuffer[1500];
	char payload_data[1500];
	int payload_data_size;

	ptr = fopen("./out.bin", "rb");  // r for read, b for binary

	fread(fbuffer, sizeof(fbuffer), 1, ptr); // read 10 bytes to our buffer

	struct MYRTP* my_rtp_class = newMYRTPClass();
	MYRTP_parsing(my_rtp_class, fbuffer, 1500, payload_data, payload_data_size);

	printf("%x\n", payload_data);

	deleteMYRTPClass(my_rtp_class);
}