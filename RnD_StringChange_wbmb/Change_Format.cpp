#include <stdio.h>
#include <string.h>

static char http_string_buf_sm[1024];

//char* http_request_buf_sm = "POST /syncpc/%s.do HTTP/1.1\r\n"
//"Host: %s:%d\r\n"
//"Connection: keep-alive\r\n"
//"Content-Length: %d\r\n"
//"User-Agent: Hanilab TX\r\n"
//"Cache-Control: no-cache\r\n"
//"Origin: hanilab-tx\r\n"
//"Content-Type: application/json\r\n"
//"Accept: */*\r\n"
//"Accept-Encoding: gzip, deflate, br\r\n"
//"Accept-Language: ko-KR,ko;q=0.8,en-US;q=0.6,en;q=0.4\r\n"
//"\r\n"
//"%s";

int main() {

	//char * str = "{\"tx-info-request\": \"\"}";

	//sprintf(http_string_buf_sm, http_request_buf_sm, "tx-info-request", "192.168.0.22", "8061", strlen(str), str);

	printf("%s", http_string_buf_sm);

	return 0;
}