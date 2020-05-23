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

struct Room;

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
    /** Thread processing packets sent by user */
    Thread thread;

    // TODO: Implement in a better way
    /* Upload */
    struct {
        unsigned int fileId;
        long long fileSize;
        long long received;
        char* fileContent;
    } uploadData[MAX_CONCURRENT_FILE_TRANSFER];
    /* Download */
    struct {
        Thread downloadThread;
        unsigned int downloadedFileId;
    } downloadData[MAX_CONCURRENT_FILE_TRANSFER];

    struct Room* room;
} Client;

typedef struct Room {
    char name[ROOM_NAME_MAX_LENGTH + 1];
    char description[ROOM_DESC_MAX_LENGTH + 1];
    Client* clients[MAX_USERS_PER_ROOM];
    Client* owner;
} Room;

extern ReadWriteLock clientsLock;
extern Client* clients[NUMBER_CLIENT_MAX];
extern Room* rooms[NUMBER_ROOM_MAX];

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
