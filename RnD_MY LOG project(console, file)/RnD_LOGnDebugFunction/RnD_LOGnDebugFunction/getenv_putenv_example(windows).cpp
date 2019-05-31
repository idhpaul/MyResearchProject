//!시스템환경변수에는 영향을 주지 않음 프로세스상의 환경변수를 지정함
//!리눅스환경 테스트 X

// crt_getenv.c
// compile with: /W3
// This program uses getenv to retrieve
// the LIB environment variable and then uses
// _putenv to change it to a new value.

#include <stdlib.h>
#include <stdio.h>

int main(void)
{
	char *libvar;

	// Get the value of the LIB environment variable.
	libvar = getenv("LIB"); // C4996
	// Note: getenv is deprecated; consider using getenv_s instead

	if (libvar != NULL)
		printf("Original LIB variable is: %s\n", libvar);

	// Attempt to change path. Note that this only affects the environment
	// variable of the current process. The command processor's
	// environment is not changed.
	_putenv("LIB=c:\\mylib;c:\\yourlib"); // C4996
	// Note: _putenv is deprecated; consider using putenv_s instead

	// Get new value.
	libvar = getenv("LIB"); // C4996

	if (libvar != NULL)
		printf("New LIB variable is: %s\n", libvar);
}