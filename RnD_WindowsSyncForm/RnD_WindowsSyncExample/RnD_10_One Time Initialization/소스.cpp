#define _WIN32_WINNT 0x0600
#include <windows.h>

// Global variable for one-time initialization structure
INIT_ONCE g_InitOnce = INIT_ONCE_STATIC_INIT; // Static initialization

// Initialization callback function 
BOOL CALLBACK InitHandleFunction(
    PINIT_ONCE InitOnce,
    PVOID Parameter,
    PVOID* lpContext);

// Returns a handle to an event object that is created only once
HANDLE OpenEventHandleSync()
{
    PVOID lpContext;
    BOOL  bStatus;

    // Execute the initialization callback function 
    bStatus = InitOnceExecuteOnce(&g_InitOnce,          // One-time initialization structure
        InitHandleFunction,   // Pointer to initialization callback function
        NULL,                 // Optional parameter to callback function (not used)
        &lpContext);          // Receives pointer to event object stored in g_InitOnce

// InitOnceExecuteOnce function succeeded. Return event object.
    if (bStatus)
    {
        return (HANDLE)lpContext;
    }
    else
    {
        return (INVALID_HANDLE_VALUE);
    }
}

// Initialization callback function that creates the event object 
BOOL CALLBACK InitHandleFunction(
    PINIT_ONCE InitOnce,        // Pointer to one-time initialization structure        
    PVOID Parameter,            // Optional parameter passed by InitOnceExecuteOnce            
    PVOID* lpContext)           // Receives pointer to event object           
{
    HANDLE hEvent;

    // Create event object
    hEvent = CreateEvent(NULL,    // Default security descriptor
        TRUE,    // Manual-reset event object
        TRUE,    // Initial state of object is signaled 
        NULL);   // Object is unnamed

// Event object creation failed.
    if (NULL == hEvent)
    {
        return FALSE;
    }
    // Event object creation succeeded.
    else
    {
        *lpContext = hEvent;
        return TRUE;
    }
}