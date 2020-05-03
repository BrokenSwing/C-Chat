/**
 * \file server.c
 * \brief Server file.
 * 
 * Allows multiple clients to discuss, relaying messages between them.
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "../common/synchronization.h"
#include "server.h"
#include "../common/packets.h"

static ReadWriteLock clientsLock;
static Client* clients[NUMBER_CLIENT_MAX] = {NULL};

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
                printf("Received invalid username.\n");
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

    Packet packet = NewPacketLeave;
    memcpy(packet.asLeavePacket.username, client->username, USERNAME_MAX_LENGTH + 1);
    broadcast(&packet);

    /* Closing connection with client */
    closeSocket(&(client->socket));

    printf("Client disconnected : %s\n", client->username);

    /* Free heap-allocated memory for client */
    free(client);
}

void broadcast(Packet* packet) {
    acquireRead(clientsLock);
    { // BEGIN CRITICAL SECTION
        for (int i = 0; i < NUMBER_CLIENT_MAX; i++) {
            Client *c = clients[i];
            if (c != NULL && c->joined) {
                sendPacket(c->socket, packet);
            }
        }
    } // END CRITICAL SECTION
    releaseRead(clientsLock);
}

void relayClientMessages(Client* client) {
    Packet packet;
    int bytesReceived;
    do {
        bytesReceived = receiveNextPacket(client->socket, &packet);
        if (bytesReceived > 0) {
            switch(packet.type) {
                case TEXT_MESSAGE_TYPE:
                    if (strlen(packet.asTextPacket.message) > 0) {
                        memcpy(packet.asTextPacket.username, client->username, USERNAME_MAX_LENGTH + 1);
                        broadcast(&packet);
                    }
                    break;
                case DEFINE_USERNAME_MESSAGE_TYPE:
                    ; // https://stackoverflow.com/questions/18496282/why-do-i-get-a-label-can-only-be-part-of-a-statement-and-a-declaration-is-not-a
                    unsigned int usernameLength = strlen(packet.asDefineUsernamePacket.username);
                    if (usernameLength > 0 && usernameLength <= USERNAME_MAX_LENGTH) {
                        Packet usernameChanged = NewPacketUsernameChanged;

                        memcpy(usernameChanged.asUsernameChangedPacket.oldUsername, client->username, USERNAME_MAX_LENGTH + 1);
                        memcpy(usernameChanged.asUsernameChangedPacket.newUsername, packet.asDefineUsernamePacket.username, USERNAME_MAX_LENGTH + 1);
                        memcpy(client->username, packet.asDefineUsernamePacket.username, USERNAME_MAX_LENGTH + 1);

                        broadcast(&usernameChanged);
                    } else {
                        Packet errorPacket = NewPacketServerErrorMessage;
                        memcpy(errorPacket.asServerErrorMessagePacket.message, "Invalid username", 17);
                        sendPacket(client->socket, &packet);
                    }
                    break;
                case QUIT_MESSAGE_TYPE:
                    return; // Other option is to set bytesReceived to -1, but we want to keep semantic of variable
            }
        }
    } while (bytesReceived >= 0);
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

    Packet packet = NewPacketJoin;
    acquireRead(clientsLock);
    { // BEGIN CRITICAL SECTION
        memcpy(packet.asJoinPacket.username, clients[id]->username, USERNAME_MAX_LENGTH + 1);
    } // END CRITICAL SECTION

    broadcast(&packet);
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
        acquireRead(clientsLock);
        int slotId = scanForFreeSocketSlot();
        releaseRead(clientsLock);

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