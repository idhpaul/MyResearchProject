#include <stdio.h>
#include <string.h>
#include "log_util.h"

#define __DEBUG_PREPROCESSER 0

int my_test_test_func(int value)
{
    LOG_TRACE("TRACE LOG...1");
    LOG_DEBUG("TEST");
	printf("%d\n", value);
    LOG_TRACE("TRACE LOG...2");
    LOG_DEBUG("%s", "sample test");
}

int main(int argc, char* argv[])
{
	int ret;
	int value = 0;

#if __DEBUG_PREPROCESSER
	printf("This Print Meaning Preprocesser is enable \n");
#endif

	LOGsetInfo(".", "LOG");
	//LOGsetInfo(".", basename(argv[0]));

    LOG_TRACE("TRACE LOG...1");
    LOG_DEBUG("TEST");
    LOG_TRACE("TRACE LOG...2");
    LOG_CALL(ret = my_test_test_func(value));
    LOG_DEBUG("%s", "sample test");
    LOG_TRACE("TRACE LOG...3");
    LOG_ERROR("%s 오류입니다.", "sample test");
    LOG_TRACE("TRACE LOG...4");
    return 0;
}
