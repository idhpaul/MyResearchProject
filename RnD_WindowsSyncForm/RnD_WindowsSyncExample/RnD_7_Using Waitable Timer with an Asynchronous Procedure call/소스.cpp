#define UNICODE 1
#define _UNICODE 1

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define _SECOND 10000000

typedef struct _MYDATA {
    TCHAR* szText;
    DWORD dwValue;
} MYDATA;

VOID CALLBACK TimerAPCProc(
    LPVOID lpArg,               // Data value
    DWORD dwTimerLowValue,      // Timer low value
    DWORD dwTimerHighValue)    // Timer high value

{
    // Formal parameters not used in this example.
    UNREFERENCED_PARAMETER(dwTimerLowValue);
    UNREFERENCED_PARAMETER(dwTimerHighValue);

    MYDATA* pMyData = (MYDATA*)lpArg;

    _tprintf(TEXT("Message: %s\nValue: %d\n\n"), pMyData->szText,
        pMyData->dwValue);
    MessageBeep(0);

}

int main(void)
{
    HANDLE          hTimer;
    BOOL            bSuccess;
    __int64         qwDueTime;
    LARGE_INTEGER   liDueTime;
    MYDATA          MyData;

    MyData.szText = TEXT("This is my data");
    MyData.dwValue = 100;

    hTimer = CreateWaitableTimer(
        NULL,                   // Default security attributes
        FALSE,                  // Create auto-reset timer
        TEXT("MyTimer"));       // Name of waitable timer
    if (hTimer != NULL)
    {
        __try
        {
            // Create an integer that will be used to signal the timer 
            // 5 seconds from now.
            qwDueTime = -5 * _SECOND;

            // Copy the relative time into a LARGE_INTEGER.
            liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
            liDueTime.HighPart = (LONG)(qwDueTime >> 32);

            bSuccess = SetWaitableTimer(
                hTimer,           // Handle to the timer object
                &liDueTime,       // When timer will become signaled
                2000,             // Periodic timer interval of 2 seconds
                TimerAPCProc,     // Completion routine
                &MyData,          // Argument to the completion routine
                FALSE);          // Do not restore a suspended system

            if (bSuccess)
            {
                for (; MyData.dwValue < 1000; MyData.dwValue += 100)
                {
                    SleepEx(
                        INFINITE,     // Wait forever
                        TRUE);       // Put thread in an alertable state
                }

            }
            else
            {
                printf("SetWaitableTimer failed with error %d\n", GetLastError());
            }

        }
        __finally
        {
            CloseHandle(hTimer);
        }
    }
    else
    {
        printf("CreateWaitableTimer failed with error %d\n", GetLastError());
    }

    return 0;
}