#ifndef C_CHAT_SYNCHRONIZATION_H
#define C_CHAT_SYNCHRONIZATION_H

typedef struct _Mutex {
    void* info;
} Mutex;

typedef struct _ReadWriteLock {
    Mutex writeLock;
    Mutex readLock;
    int* readCount;
} ReadWriteLock;

Mutex createMutex();
void acquireMutex(Mutex mutex);
void releaseMutex(Mutex mutex);
void destroyMutex(Mutex mutex);

ReadWriteLock createReadWriteLock();
void acquireRead(ReadWriteLock lock);
void acquireWrite(ReadWriteLock lock);
void releaseRead(ReadWriteLock lock);
void releaseWrite(ReadWriteLock lock);
void destroyReadWriteLock(ReadWriteLock lock);

#endif //C_CHAT_SYNCHRONIZATION_H
