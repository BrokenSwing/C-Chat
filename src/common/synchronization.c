#include "synchronization.h"
#include <stdio.h>
#include <stdlib.h>
#include "interop.h"

ReadWriteLock createReadWriteLock() {
    ReadWriteLock lock;
    lock.readCount = malloc(sizeof(int));
    *(lock.readCount) = 0;
    lock.readLock = createMutex();
    lock.writeLock = createMutex();

    return lock;
}

void acquireRead(ReadWriteLock lock) {
    acquireMutex(lock.readLock);
    (*(lock.readCount))++;

    if (*(lock.readCount) == 1) {
        acquireMutex(lock.writeLock);
    }

    releaseMutex(lock.readLock);
}

void releaseRead(ReadWriteLock lock) {
    acquireMutex(lock.readLock);
    (*(lock.readCount))--;

    if (*(lock.readCount) == 0) {
        releaseMutex(lock.writeLock);
    }

    releaseMutex(lock.readLock);
}

void acquireWrite(ReadWriteLock lock) {
    acquireMutex(lock.writeLock);
}

void releaseWrite(ReadWriteLock lock) {
    releaseMutex(lock.writeLock);
}

void destroyReadWriteLock(ReadWriteLock lock) {
    destroyMutex(lock.readLock);
    destroyMutex(lock.writeLock);
    free(lock.readCount);
}

#if IS_POSIX

    #include <pthread.h>

    struct UnixMutex {
        pthread_mutex_t mutex;
    };

    Mutex createMutex() {
        struct UnixMutex* unixMutex = malloc(sizeof(struct UnixMutex));
        pthread_mutex_init(&(unixMutex->mutex), NULL);

        Mutex m;
        m.info = unixMutex;

        return m;
    }

    void acquireMutex(Mutex mutex) {
        struct UnixMutex* unixMutex = mutex.info;
        pthread_mutex_lock(&(unixMutex->mutex));
    }

    void releaseMutex(Mutex mutex) {
        struct UnixMutex* unixMutex = mutex.info;
        pthread_mutex_unlock(&(unixMutex->mutex));
    }

    void destroyMutex(Mutex mutex) {
        struct UnixMutex* unixMutex = mutex.info;
        pthread_mutex_destroy(&(unixMutex->mutex));
        free(unixMutex);
    }


#elif IS_WINDOWS

    #ifndef WINDOWS_LEAN_AND_MEAN
    #define WINDOWS_LEAN_AND_MEAN
    #include <windows.h>
    #endif

    struct WinMutex {
        HANDLE handle;
    };

    Mutex createMutex() {
        HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
        if (mutex == NULL) {
            printf("Unable to create mutex. Error code : %ld\n", GetLastError());
            exit(EXIT_FAILURE);
        }

        struct WinMutex* winMutex = malloc(sizeof(struct WinMutex));
        winMutex->handle = mutex;

        Mutex m;
        m.info = winMutex;

        return m;
    }

    void acquireMutex(Mutex mutex) {
        struct WinMutex* winMutex = mutex.info;
        WaitForSingleObject(winMutex->handle, INFINITE);
    }

    void releaseMutex(Mutex mutex) {
        struct WinMutex* winMutex = mutex.info;
        ReleaseMutex(winMutex->handle);
    }

    void destroyMutex(Mutex mutex) {
        struct WinMutex* winMutex = mutex.info;
        CloseHandle(winMutex->handle);
        free(winMutex);
    }

#endif

