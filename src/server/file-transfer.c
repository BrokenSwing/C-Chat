#include "file-transfer.h"
#include <stdlib.h>
#include "../common/files.h"
#include <stdio.h>
#include "communication.h"

static unsigned int nextFileId = 1; // TODO: synchronize access

// Simple implementation
unsigned int generateNewFileId() {
    return nextFileId++;
}

void handleUploadRequest(Client* client, struct PacketFileUploadRequest* packet) {
    if (client->fileId != 0) {
        Packet response = NewPacketFileTransferValidation;
        response.asFileTransferValidationPacket.accepted = 0;
        response.asFileTransferValidationPacket.id = 0;
        sendPacket(client->socket, &response);

        response = NewPacketServerErrorMessage;
        memcpy(response.asServerErrorMessagePacket.message, "Already uploading.", 19);
        sendPacket(client->socket, &response);
    } else if (packet->fileSize <= 0 || packet->fileSize > MAX_FILE_SIZE_UPLOAD) {
        Packet response = NewPacketFileTransferValidation;
        response.asFileTransferValidationPacket.accepted = 0;
        response.asFileTransferValidationPacket.id = 0;
        sendPacket(client->socket, &response);

        response = NewPacketServerErrorMessage;
        memcpy(response.asServerErrorMessagePacket.message, "File too large.", 16);
        sendPacket(client->socket, &response);

    } else {
        unsigned int fileId = generateNewFileId();

        Packet response = NewPacketFileTransferValidation;
        response.asFileTransferValidationPacket.accepted = 1;
        response.asFileTransferValidationPacket.id = fileId;
        sendPacket(client->socket, &response);

        client->fileId = fileId;
        client->fileContent = malloc(sizeof(char) * packet->fileSize);
        client->fileSize = packet->fileSize;
        client->received = 0;
    }
}

void handleFileDataUpload(Client* client, struct PacketFileDataTransfer* packet) {
    if (client->fileId > 0 && packet->id == client->fileId) {
        unsigned remainingToDownload = client->fileSize - client->received;
        unsigned int nextChunkSize = remainingToDownload > FILE_TRANSFER_CHUNK_SIZE ? FILE_TRANSFER_CHUNK_SIZE : remainingToDownload;
        memcpy(client->fileContent + client->received, packet->data, nextChunkSize);
        client->received += nextChunkSize;

        if (client->received >= client->fileSize) {

            /* Writing file to disk */
            char filename[12];
            sprintf(filename, "%d", client->fileId);
            files_writeFile(filename, client->fileContent, client->fileSize);

            /* Telling clients a new file is available */
            Packet uploadSuccessPacket = NewPacketServerErrorMessage; // TODO: Create a ServerInformation packet
            sprintf(uploadSuccessPacket.asServerErrorMessagePacket.message, "%s uploaded file %d", client->username, client->fileId);
            broadcast(&uploadSuccessPacket);

            client->fileId = 0;
            client->received = 0;
            free(client->fileContent);
            client->fileContent = NULL;
        }
    } // Just ignoring packet if id does not match
}