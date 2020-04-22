/**
 * \file server.c
 * \brief Server file.
 * 
 * Allows multiple clients to discuss, relaying messages between them.
 * 
 */

#include "../common/sockets.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "../common/threads.h"
#include "../common/constants.h"
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
typedef struct _Client {
    /** The socket from server to the client */
    SocketInfo socket;
    /** A buffer meant to contain client username */
    char username[USERNAME_MAX_LENGTH + 1];
    /**
     * A short indicating whether or not, the client joined discussion.
     * Equal to 0 if client isn't in the discussion, else 1.
     */
    short joined;
    /** Thread relaying messages sent by user */
    Thread thread;
} Client;

static ReadWriteLock clientsLock;
static Client* clients[NUMBER_CLIENT_MAX] = {NULL};

/**
 * \brief Scan clients slots for an available slot.
 *
 * Iterates over clients slots to find an empty slot.
 *
 * \return -1 if no empty slot was found, else an available slot id
 */
int scanForFreeSocketSlot();

/**
 * \brief Catch interrupt signal.
 *
 * \param signal Incoming signal
 */
void handleServerClose(int signal);

/**
 * \brief Initialize a client connection to be ready to discuss.
 *
 * Receives the client username and performs checks to ensure its validity.
 *
 * \param client A pointer to an integer which contains the slot id of the client to initialize
 * \return EXIT_SUCCESS if client initialized correctly, else EXIT_FAILURE
 */
int initClientConnection(Client* client);

/**
 * \brief Disconnects the client and free allocated memory
 *
 * \param id The client slot id
 */
void disconnectClient(int id);

/**
 * \brief Verifies if the end of the communication is requested.
 *
 * \param buffer A message.
 * \return 1 if the message is "fin", else 0.
 */
int receivedEndMessage(const char* buffer);

/**
 * \brief Broadcasts a message to every clients
 *
 * @param buffer The message to broacast
 * @param size The message length
 */
void broadcast(const char* buffer, int size);

/**
 * \brief Relay messages sent by given client to all known clients.
 *
 * Waits for an incoming message, and broadcast it (in a correctly formed relayed message) to all
 * known clients.
 *
 * @param client The client to wait messages from
 */
void relayClientMessages(Client* client);

int scanForFreeSocketSlot() {
    int i = 0;
    while(i < NUMBER_CLIENT_MAX && clients[i] != NULL) {
        i++;
    }
    return i == NUMBER_CLIENT_MAX ? -1 : i;
}

void handleServerClose(int signal) {
    for (int i = 0; i < NUMBER_CLIENT_MAX; i++) {
        Client* client = clients[i];
        if (client != NULL) {
            clients[i] = NULL;
            closeSocket(&(client->socket));
            destroyThread(&(client->thread));
            free(client);
        }
    }
    cleanUp();
    destroyReadWriteLock(clientsLock);
    printf("Server closed.\n");
    exit(EXIT_SUCCESS);
}

int initClientConnection(Client* client) {
    /* Message sent when receiving empty username */
    const char* emptyUsername = "Error. Username can't be empty.";
    const char* okUsername = "Ok";

    SocketInfo socket = client->socket;

    short validUsername = 0;
    int bytesReceived;
    do {
        /* Receiving client username */
        bytesReceived = receiveFrom(socket, client->username, USERNAME_MAX_LENGTH);

        /* If we received data */
        if (bytesReceived >= 0) {
            client->username[bytesReceived] = '\0';

            /* We don't allow empty username */
            if (strlen(client->username) == 0) {
                sendTo(socket, emptyUsername, strlen(emptyUsername));
            } else {
                validUsername = 1;
            }
        }
    } while(!validUsername && bytesReceived >= 0); // Keep iterating while we receive data and username is invalid

    if (validUsername) {
        /* Telling client its username is valid */
        sendTo(client->socket, "Ok", strlen(okUsername));
        acquireWrite(clientsLock);
        // BEGIN CRITICAL SECTION
        client->joined = 1;
        // END CRITICAL SECTION
        releaseWrite(clientsLock);
        printf("A client connected with username: %s\n", client->username);
    }
    return validUsername ? EXIT_SUCCESS : EXIT_FAILURE;
}

void disconnectClient(int id) {
    acquireWrite(clientsLock);
    // BEGIN CRITICAL SECTION
    Client* client = clients[id];
    clients[id] = NULL;
    // END CRITICAL SECTION
    releaseWrite(clientsLock);

    /* Closing connection with client */
    closeSocket(&(client->socket));

    printf("Client disconnected : %s\n", client->username);

    /* Free heap-allocated memory for client */
    free(client);
}

int receivedEndMessage(const char* buffer) {
    return buffer[0] == 'f' && buffer[1] == 'i' && buffer[2] == 'n' && buffer[3] == '\0';
}

void broadcast(const char* buffer, int size) {
    acquireRead(clientsLock);
    // BEGIN CRITICAL SECTION
    for (int i = 0; i < NUMBER_CLIENT_MAX; i++) {
        Client* c = clients[i];
        if (c != NULL && c->joined) {
            sendTo(c->socket, buffer, size);
        }
    }
    // END CRITICAL SECTION
    releaseRead(clientsLock);
}

void relayClientMessages(Client* client) {
    char buffer[MESSAGE_TYPE_OVERHEAD + (MSG_MAX_LENGTH + 1) + (USERNAME_MAX_LENGTH + 1)];
    buffer[0] = TEXT_MESSAGE_TYPE;
    memcpy(buffer + MESSAGE_TYPE_OVERHEAD + MSG_MAX_LENGTH + 1, client->username, sizeof(client->username));

    int bytesReceived;
    do {
        bytesReceived = receiveFrom(client->socket, buffer, MESSAGE_TYPE_OVERHEAD + MSG_MAX_LENGTH);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            if (strlen(buffer) > 0 && !receivedEndMessage(buffer)) {
                broadcast(buffer, MESSAGE_TYPE_OVERHEAD + (MSG_MAX_LENGTH + 1) + (USERNAME_MAX_LENGTH + 1));
            }
        }
    } while (bytesReceived >= 0 && !receivedEndMessage(buffer + MESSAGE_TYPE_OVERHEAD));
}

THREAD_ENTRY_POINT clientThread(void* idPnt) {
    /* Retrieving client's slot id to initialize, and remove heap-allocated int */
    int id = *((int*)idPnt);
    free(idPnt);

    Client* client = clients[id];

    int success = initClientConnection(client);
    if (success == EXIT_FAILURE) {
        disconnectClient(id);
        return EXIT_FAILURE;
    }

    char* joinMessage = malloc(sizeof(char) * (MESSAGE_TYPE_OVERHEAD + USERNAME_MAX_LENGTH + 1));
    joinMessage[0] = JOIN_MESSAGE_TYPE;
    acquireRead(clientsLock);
    { // BEGIN CRITICAL SECTION
        memcpy(joinMessage + MESSAGE_TYPE_OVERHEAD, clients[id]->username, USERNAME_MAX_LENGTH + 1);
    } // END CRITICAL SECTION

    broadcast(joinMessage, MESSAGE_TYPE_OVERHEAD + USERNAME_MAX_LENGTH + 1);
    relayClientMessages(client);
    disconnectClient(id);

    return EXIT_SUCCESS;
}

/**
 * \brief Program entry point.
 */
int main () {
    clientsLock = createReadWriteLock();

    /* Create server socket */
    SocketInfo serverSocket = createServerSocket("27015");
    /* Capture interruption signal to be able to cleanup allocated resources when server stops */
    signal(SIGINT, handleServerClose);

    printf("Server ready to accept connections.\n");

    while(1) {
        /* Waiting for a client to connect */
        SocketInfo clientSocket = acceptClient(serverSocket);

        /* Looking for a valid id for connected client */
        int slotId = scanForFreeSocketSlot();

        /* If no valid slot id was found, closing connection with client */
        if (slotId == -1) {
            printf("Accepted client but we're full. Closing connection.\n");
            const char* full = "Full.";
            sendTo(clientSocket, full, strlen(full));
            closeSocket(&clientSocket);
            continue;
        }

        printf("Found slot %d for the new client.\n", slotId);

        /* Allocating memory for client */
        Client *client = malloc(sizeof(Client)); // Free-ed in disconnectClient function
        client->socket = clientSocket;
        client->joined = 0;
        acquireWrite(clientsLock);
        clients[slotId] = client;
        releaseWrite(clientsLock);

        /* Create thread to initialize connection with client and passing client slot id to this thread */
        int* id = malloc(sizeof(int)); // Free-ed in clientThread function
        *id = slotId;
        Thread thread = createThread(clientThread, id);
        client->thread = thread;
    }
}