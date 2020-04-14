/**
 * \file threads.h
 * \brief An OS-agnostic API to manipulate threads.
 */

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

/**
 * \brief A macro to declare a function meant to be used as an entry point for a thread.
 */
#define THREAD_ENTRY_POINT THREAD_RETURN_TYPE THREAD_CALL_TYPE

typedef THREAD_RETURN_TYPE (THREAD_CALL_TYPE *FP_THREAD) (void*);

/**
 * \brief The type for a valid thread entry point.
 */
typedef FP_THREAD THREAD_FUNCTION_POINTER;

/**
 * \struct Thread
 * \brief Represents a thread
 *
 * Underlying information are OS-specifics
 */
typedef struct Thread {
    void* info;
} Thread;

/**
 * \brief Creates and starts a thread.
 *
 * Creates a thread which the entry point will be the given function.
 * The given entry point MUST be declared as a thread entry point.
 * Example :
 *
 *  THREAD_ENTRY_POINT entryPointName(void* data) {
 *      return EXIT_SUCCESS;
 *  }
 *
 * \param entryPoint The entry point for the thread
 * \return the created thread
 */
Thread createThread(THREAD_FUNCTION_POINTER entryPoint);

/**
 * \brief Destroys the given thread.
 *
 * \param thread The thread to destroy
 */
void destroyThread(Thread thread);

/**
 * \brief Waits for the given thread to finish.
 *
 * \param thread The thread to wait
 */
void joinThread(Thread thread);

#endif //C_CHAT_THREADS_H
