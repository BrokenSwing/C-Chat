#include "threads.h"
#include <stdio.h>

#if defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__linux__)

#elif defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS)

#ifndef WIN32_LEAN_AND_MEAN
#include <windows.h>
#define WIN32_LEAN_AND_MEAN
#endif

#define THREAD_RETURN_TYPE DWORD
#define THREAD_CALL_TYPE WINAPI

struct WinThread {
    DWORD id;
    HANDLE handle;
};

Thread createThread(THREAD_FUNCTION_POINTER entryPoint) {
    DWORD threadId;
    HANDLE threadHandle = CreateThread(NULL, 0, entryPoint, NULL, 0, &threadId);
    if (threadHandle == NULL) {
        printf("Unable to create thread. Error code : %d\n", GetLastError());
        exit(EXIT_FAILURE);
    }

    struct WinThread *winThread = malloc(sizeof(struct WinThread));
    winThread->id = threadId;
    winThread->handle = threadHandle;

    struct Thread thread;
    thread.info = winThread;

    return thread;
}

void destroyThread(Thread thread) {
    struct WinThread *t = thread.info;
    CloseHandle(t->handle);
}

void joinThread(Thread thread) {
    struct WinThread *t = thread.info;
    WaitForSingleObject(t->handle, INFINITE);
    destroyThread(thread);
}

#endif