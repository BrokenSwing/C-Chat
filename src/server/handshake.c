#include "handshake.h"
#include <stdio.h>
#include <stdlib.h>
#include "communication.h"

int scanForFreeSocketSlot() {
    int i = 0;
    while(i < NUMBER_CLIENT_MAX && clients[i] != NULL) {
        i++;
    }
    return i == NUMBER_CLIENT_MAX ? -1 : i;
}

int initClientConnection(Client* client) {
    /* Message sent when receiving empty username */
    const char* emptyUsername = "Error. Username can't be empty.";
    const char* okUsername = "Ok";

    Socket socket = client->socket;

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
        SYNC_CLIENT_WRITE(client->joined = 1);
        printf("A client connected with username: %s\n", client->username);
    }
    return validUsername ? EXIT_SUCCESS : EXIT_FAILURE;
}

void disconnectClient(int id) {
    SYNC_CLIENT_WRITE(
            Client* client = clients[id];
            clients[id] = NULL;
    )

    Packet packet = NewPacketLeave;
    memcpy(packet.asLeavePacket.username, client->username, USERNAME_MAX_LENGTH + 1);
    broadcast(&packet);

    /* Closing connection with client */
    closeSocket(&(client->socket));

    printf("Client disconnected : %s\n", client->username);

    /* Free heap-allocated memory for client */
    free(client);
}
