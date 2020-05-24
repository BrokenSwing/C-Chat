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

    /**
     * A pointer to the room the client joined. Can be NULL.
     *
     * It can be written to NULL by room-owner thread at any time if the room owner disband deletes the room.
     * It can be written to NULL by self thread at any time if the client asks to leave the room.
     * It can be written to non-NULL value ONLY by self thread when the client asks to create a join a room.
     * It can be read by self thread at any time to relay packets to clients in the room.
     *
     * We MUST acquire clientsLock to access this field.
     */
    struct Room* room;
} Client;

typedef struct Room {
    char name[ROOM_NAME_MAX_LENGTH + 1];
    char description[ROOM_DESC_MAX_LENGTH + 1];
    /**
     * An array of pointers to clients who joined the room.
     *
     * It can be written by any client thread at any time if a client joins the room or leaves the room.
     * It can be read by any client at any time to relay a packet.
     *
     * We MUST acquire lock field to access this field.
     */
    Client* clients[MAX_USERS_PER_ROOM];
    Client* owner;
    ReadWriteLock lock;
} Room;

extern ReadWriteLock clientsLock;
extern ReadWriteLock roomsLock;
extern Client* clients[NUMBER_CLIENT_MAX];
/**
 * An array containing all pointers to existing rooms.
 *
 * It can be written by any client thread at any time if a client creates or deletes a room.
 * It can be read by any client thread at any time if a client joins a room.
 *
 * We MUST acquire roomsLock to access this field.
 */
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
 * \def SYNC_ROOMS_READ
 * \brief Macro to synchronize rooms read operation
 */
#define SYNC_ROOMS_READ(op)       \
acquireRead(roomsLock);           \
op;                               \
releaseRead(roomsLock);

/**
 * \def SYNC_ROOMS_WRITE
 * \brief Macro to synchronize rooms write operation
 */
#define SYNC_ROOMS_WRITE(op)      \
acquireWrite(roomsLock);          \
op;                               \
releaseWrite(roomsLock);

/**
 * \def SYNC_ROOM_READ
 * \brief Macro to synchronize read operation on a single room
 */
#define SYNC_ROOM_READ(room, op)  \
acquireRead(room->lock);          \
op;                               \
releaseRead(room->lock);

/**
 * \def SYNC_ROOM_WRITE
 * \brief Macro to synchronize write operation on a single room
 */
#define SYNC_ROOM_READ(room, op)  \
acquireRead(room->lock);          \
op;                               \
releaseRead(room->lock);

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
