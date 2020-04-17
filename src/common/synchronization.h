/**
 * \file synchronization.h
 * \brief Provides types allowing to synchronize access to resources.
 */

#ifndef C_CHAT_SYNCHRONIZATION_H
#define C_CHAT_SYNCHRONIZATION_H

/**
 * \class Mutex
 * \brief A semaphore allowing mutual exclusion.
 */
typedef struct _Mutex {
    void* info;
} Mutex;

/**
 * \class ReadWriteLock
 * \brief A read/write lock to synchronize resource access.
 */
typedef struct _ReadWriteLock {
    Mutex writeLock;
    Mutex readLock;
    int* readCount;
} ReadWriteLock;

/**
 * \brief Creates a mutex.
 *
 * \return a ready to use mutex
 */
Mutex createMutex();

/**
 * \brief Acquires the given mutex.
 *
 * Once acquired, a mutex MUST be released by the thread which acquired it before any
 * thread can re-acquiring.
 * This is a blocking-call.
 *
 * \param mutex The mutex to acquire
 */
void acquireMutex(Mutex mutex);

/**
 * \brief Releases the given mutex.
 *
 * The given mutex MUST be acquired by the current thread before releasing.
 *
 * \param mutex The mutex to release
 */
void releaseMutex(Mutex mutex);

/**
 * \brief Destroys the given mutex.
 *
 * WARNING : A mutex MUST NOT be destroyed twice. It would lead to unexpected behavior.
 *
 * \param mutex The mutex to destroy
 */
void destroyMutex(Mutex mutex);

/**
 * \brief Creates a read/write lock.
 *
 * A read/write lock allow to protect access to a resource.
 * When acquiring read lock, no thread acquire write lock but all threads can still
 * acquire read lock.
 * When acquiring write lock, no thread can acquire any lock before lock release.
 *
 * It allows to ensure resources is not read while writing it.
 *
 * \return a read/write lock
 */
ReadWriteLock createReadWriteLock();

/**
 * \brief Acquires read lock on the given read/write lock.
 *
 * This is a blocking call.
 * It waits for the read lock to be available, this one can be unavailable if
 * write lock is acquired.
 *
 * \param lock The read/write lock to acquire read on
 */
void acquireRead(ReadWriteLock lock);

/**
 * \brief Acquires write lock on the given read/write lock.
 *
 * This is a blocking call.
 * It wait for the read lock to be available, this one can be unavailable if
 * a thread already acquired write lock or if any thread acquired read lock.
 *
 * \param lock The read/write lock to acquire write on
 */
void acquireWrite(ReadWriteLock lock);

/**
 * \brief Releases read lock on the given read/write lock.
 *
 * The lock MUST have been acquired by this thread before.
 *
 * \param lock The read/write lock to release read of
 */
void releaseRead(ReadWriteLock lock);

/**
 * \brief Releases write lock on the given read/write lock.
 *
 * The lock MUST have been acquired by this thread before.
 *
 * \param lock The read/write lock to release write of
 */
void releaseWrite(ReadWriteLock lock);

/**
 * \brief Destroys the given read/write lock.
 *
 * It frees allocated memory for the given read/write lock.
 *
 * WARNING: This function MUST NOT be called twice for the same lock. It would lead
 * to unexpected behavior.
 *
 * @param lock The read/write lock to destroy
 */
void destroyReadWriteLock(ReadWriteLock lock);

#endif //C_CHAT_SYNCHRONIZATION_H
