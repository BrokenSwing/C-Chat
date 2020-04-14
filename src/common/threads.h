#ifndef C_CHAT_THREADS_H
#define C_CHAT_THREADS_H

#if defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__linux__)

#define THREAD_RETURN_TYPE void*
#define THREAD_CALL_TYPE

#elif defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS)

#ifndef WIN32_LEAN_AND_MEAN
#include <windows.h>
#define WIN32_LEAN_AND_MEAN
#endif

#define THREAD_RETURN_TYPE DWORD
#define THREAD_CALL_TYPE WINAPI

#endif

#define THREAD_ENTRY_POINT THREAD_RETURN_TYPE THREAD_CALL_TYPE
typedef THREAD_RETURN_TYPE (THREAD_CALL_TYPE *FP_THREAD) (void*);
typedef FP_THREAD THREAD_FUNCTION_POINTER;

typedef struct Thread {
    void* info;
} Thread;

Thread createThread(THREAD_FUNCTION_POINTER entryPoint);
void destroyThread(Thread thread);
void joinThread(Thread thread);

#endif //C_CHAT_THREADS_H
