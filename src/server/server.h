/**
 * \file server.h
 * \brief Header file for server.c
 */

#ifndef C_CHAT_CLIENT_H
#define C_CHAT_CLIENT_H

#include "../common/sockets.h"
#include "../common/constants.h"
#include "../common/threads.h"
#include "../common/packets.h"
#include "../common/synchronization.h"

/**
 * \def NUMBER_CLIENT_MAX
 * \brief Maximum of simultaneous connected clients
 */
#define NUMBER_CLIENT_MAX 10

/**
 * \class Client
 * \brief A type representing a connected client
 */
typedef struct Client {
    /** The socket from server to the client */
    Socket socket;
    /** A buffer meant to contain client username */
    char username[USERNAME_MAX_LENGTH + 1];
    /**
     * A short indicating whether or not, the client joined discussion.
     * Equal to 0 if client isn't in the discussion, else 1.
     */
    short joined;
    /** Thread relaying messages sent by user */
    Thread thread;
    unsigned int fileId;
    long long fileSize;
    long long received;
    char* fileContent;
} Client;

extern ReadWriteLock clientsLock;
extern Client* clients[NUMBER_CLIENT_MAX];

/**
 * \def SYNC_CLIENT_READ
 * \brief Macro to synchronize clients read operation
 */
#define SYNC_CLIENT_READ(op)     \
acquireRead(clientsLock);        \
op;                              \
releaseRead(clientsLock);

/**
 * \def SYNC_CLIENT_WRITE
 * \brief Macro to synchronize clients write operation
 */
#define SYNC_CLIENT_WRITE(op)     \
acquireWrite(clientsLock);        \
op;                               \
releaseWrite(clientsLock);

/**
 * \brief Catch interrupt signal.
 *
 * \param signal Incoming signal
 */
void handleServerClose(int signal);

/**
 * \brief Relay messages sent by given client to all known clients.
 *
 * Waits for an incoming message, and broadcast it (in a correctly formed relayed message) to all
 * known clients.
 *
 * \param client The client to wait messages from
 */
void handleClientsPackets(Client* client);

#endif //C_CHAT_CLIENT_H
