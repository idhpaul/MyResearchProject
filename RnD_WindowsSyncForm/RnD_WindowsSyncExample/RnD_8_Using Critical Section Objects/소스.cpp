// Global variable
CRITICAL_SECTION CriticalSection;

int main(void)
{
    ...

        // Initialize the critical section one time only.
        if (!InitializeCriticalSectionAndSpinCount(&CriticalSection,
            0x00000400))
            return;
    ...

        // Release resources used by the critical section object.
        DeleteCriticalSection(&CriticalSection);
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    ...

        // Request ownership of the critical section.
        EnterCriticalSection(&CriticalSection);

    // Access the shared resource.

    // Release ownership of the critical section.
    LeaveCriticalSection(&CriticalSection);

    ...
        return 1;
}