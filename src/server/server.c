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
#include "server.h"
#include "../common/packets.h"
#include "communication.h"
#include "handshake.h"
#include "client-info.h"
#include "file-transfer.h"
#include "room.h"

ReadWriteLock clientsLock;
Client* clients[NUMBER_CLIENT_MAX] = {NULL};
Room* rooms[NUMBER_ROOM_MAX] = {NULL};

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
    fileTransfer_cleanUp();
    destroyReadWriteLock(clientsLock);
    printf("Server closed.\n");
    exit(EXIT_SUCCESS);
}

void handleClientsPackets(Client* client) {
    Packet packet;
    int bytesReceived;
    do {
        bytesReceived = receiveNextPacket(client->socket, &packet);
        if (bytesReceived > 0) {
            switch(packet.type) {
                case TEXT_MESSAGE_TYPE:
                    handleTextMessageRelay(client, &packet.asTextPacket);
                    break;
                case DEFINE_USERNAME_MESSAGE_TYPE:
                    handleUsernameChange(client, &packet.asDefineUsernamePacket);
                    break;
                case QUIT_MESSAGE_TYPE:
                    return; // Other option is to set bytesReceived to -1, but we want to keep semantic of variable
                case FILE_UPLOAD_REQUEST_MESSAGE_TYPE:
                    handleUploadRequest(client, &packet.asFileUploadRequestPacket);
                    break;
                case FILE_DATA_TRANSFER_MESSAGE_TYPE:
                    handleFileDataUpload(client, &packet.asFileDataTransferPacket);
                    break;
                case FILE_DOWNLOAD_REQUEST_MESSAGE_TYPE:
                    handleDownloadRequest(client, &packet.asFileDownloadRequestPacket);
                    break;
                case CREATE_ROOM_MESSAGE_TYPE:
                    handleRoomCreationRequest(client, &packet.asCreateRoomPacket);
                    break;
                case JOIN_ROOM_MESSAGE_TYPE:
                    handleRoomJoinRequest(client, &packet.asJoinRoomPacket);
                    break;
                default:
                    printf("Received a packet of type %d. Can't handle this type of packet.\n", packet.type);
                    break;
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

    handleClientsPackets(client);
    disconnectClient(id);

    return EXIT_SUCCESS;
}

/**
 * \brief Program entry point.
 */
int main () {
    /* Create server socket */
    Socket serverSocket = createServerSocket("27015");
    /* Capture interruption signal to be able to cleanup allocated resources when server stops */
    signal(SIGINT, handleServerClose);

    /* Initialize systems */
    clientsLock = createReadWriteLock();
    fileTransfer_init();

    printf("Server ready to accept connections.\n");

    while(1) {
        /* Waiting for a client to connect */
        Socket clientSocket = acceptClient(serverSocket);

        /* Looking for a valid id for connected client */
        SYNC_CLIENT_READ(int slotId = scanForFreeSocketSlot());

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
        client->room = NULL;
        for (int i = 0; i < MAX_CONCURRENT_FILE_TRANSFER; i++) {
            client->uploadData[i].fileId = 0;
            client->uploadData[i].fileContent = NULL;
            client->uploadData[i].fileSize = 0;
            client->uploadData[i].received = 0;

            client->downloadData[i].downloadedFileId = 0;
        }

        SYNC_CLIENT_WRITE(clients[slotId] = client);

        /* Create thread to initialize connection with client and passing client slot id to this thread */
        int* id = malloc(sizeof(int)); // Free-ed in clientThread function
        *id = slotId;
        Thread thread = createThread(clientThread, id);
        client->thread = thread;
    }
}