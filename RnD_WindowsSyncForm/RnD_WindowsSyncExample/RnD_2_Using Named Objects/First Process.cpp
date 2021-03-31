#include <windows.h>
#include <stdio.h>
#include <conio.h>

// This process creates the mutex object.

int main(void)
{
    HANDLE hMutex;

    hMutex = CreateMutex(
        NULL,                        // default security descriptor
        FALSE,                       // mutex not owned
        TEXT("NameOfMutexObject"));  // object name

    if (hMutex == NULL)
        printf("CreateMutex error: %d\n", GetLastError());
    else
        if (GetLastError() == ERROR_ALREADY_EXISTS)
            printf("CreateMutex opened an existing mutex\n");
        else printf("CreateMutex created a new mutex.\n");

    // Keep this process around until the second process is run
    _getch();

    CloseHandle(hMutex);

    return 0;
}