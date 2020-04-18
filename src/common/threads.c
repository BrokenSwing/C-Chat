#include "threads.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__linux__)

#include <pthread.h>

struct PosixThread {
    pthread_t id;
};

Thread createThread(THREAD_FUNCTION_POINTER entryPoint, void* data) {
    pthread_t tId;
    pthread_create(&tId, 0, entryPoint, data);

    struct PosixThread *posixThread = malloc(sizeof(struct PosixThread));
    posixThread->id = tId;

    Thread t;
    t.info = posixThread;

    return t;
}

void joinThread(Thread* thread) {
    struct PosixThread *posixThread = thread->info;
    pthread_join(posixThread->id, 0);
    destroyThread(thread);
}

void destroyThread(Thread* thread) {
    struct PosixThread *posixThread = thread->info;
    if (posixThread != NULL) {
        pthread_cancel(posixThread->id);
        free(posixThread);
        thread->info = NULL;
    }
}

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

Thread createThread(THREAD_FUNCTION_POINTER entryPoint, void* data) {
    DWORD threadId;
    HANDLE threadHandle = CreateThread(NULL, 0, entryPoint, data, 0, &threadId);
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

void destroyThread(Thread* thread) {
    struct WinThread *t = thread->info;
    if (t != NULL) {
        CloseHandle(t->handle);
        free(t);
        thread->info = NULL;
    }
}

void joinThread(Thread* thread) {
    struct WinThread *t = thread->info;
    WaitForSingleObject(t->handle, INFINITE);
    destroyThread(thread);
}

#endif