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
    int alreadyUploading = client->fileId != 0;
    int fileTooLarge = packet->fileSize <= 0 || packet->fileSize > MAX_FILE_SIZE_UPLOAD;
    if (alreadyUploading || fileTooLarge) {
        /* The client is already uploading a file or file is too large. Refusing file upload */

        /* Create refuse packet */
        Packet response = NewPacketFileUploadValidation;
        struct PacketFileUploadValidation* validationPacket = &response.asFileUploadValidationPacket;

        /* Set packet attributes */
        validationPacket->accepted = 0;
        validationPacket->id = 0;

        /* Send packet to client */
        sendPacket(client->socket, &response);

        /* Tell the client the reason the upload is refused */

        /* Create packet */
        response = NewPacketServerErrorMessage;

        /* Set error message */
        if (alreadyUploading) {
            memcpy(response.asServerErrorMessagePacket.message, "Already uploading.", 19);
        } else {
            memcpy(response.asServerErrorMessagePacket.message, "File too large.", 16);
        }

        /* Send packet to client */
        sendPacket(client->socket, &response);
    } else {
        /* The file can be uploaded. Generating a file ID */
        unsigned int fileId = generateNewFileId();

        /* Create the packet */
        Packet response = NewPacketFileUploadValidation;
        struct PacketFileUploadValidation* validationPacket = &response.asFileUploadValidationPacket;

        /* Set packet attributes */
        validationPacket->accepted = 1;
        validationPacket->id = fileId;

        /* Send packet to client */
        sendPacket(client->socket, &response);

        /* Set client upload state */
        client->fileId = fileId;
        client->fileContent = malloc(sizeof(char) * packet->fileSize);
        client->fileSize = packet->fileSize;
        client->received = 0;
    }
}

void handleFileDataUpload(Client* client, struct PacketFileDataTransfer* packet) {
    if (client->fileId > 0 && packet->id == client->fileId) {
        /* The client is uploading and the packet data refers to the current upload file */

        /* Calculating expected data chunk size */
        unsigned remainingToDownload = client->fileSize - client->received;
        unsigned int nextChunkSize = remainingToDownload > FILE_TRANSFER_CHUNK_SIZE ? FILE_TRANSFER_CHUNK_SIZE : remainingToDownload;

        /* Appending data to file content */
        memcpy(client->fileContent + client->received, packet->data, nextChunkSize);
        client->received += nextChunkSize;

        if (client->received >= client->fileSize) {
            /* We received all file content */

            /* Writing file to disk */
            char filename[12];
            sprintf(filename, "%d", client->fileId);
            files_writeFile(filename, client->fileContent, client->fileSize);

            /* Telling clients a new file is available */
            Packet uploadSuccessPacket = NewPacketServerErrorMessage; // TODO: Create a ServerInformation packet
            sprintf(uploadSuccessPacket.asServerErrorMessagePacket.message, "%s uploaded file %d", client->username, client->fileId);
            broadcast(&uploadSuccessPacket);

            /* Set client upload state */
            free(client->fileContent);
            client->fileId = 0;
            client->received = 0;
            client->fileContent = NULL;
        }
    } // Just ignoring packet if id does not match
}

THREAD_ENTRY_POINT uploadFileToClient(void* data) {
    Client* client = (Client*)data;

    char filename[12];
    sprintf(filename, "%d", client->downloadedFileId);
    FileInfo info = files_getInfo(filename);

    if (info.exists && !info.isDirectory) {
        char* fileContent = malloc(sizeof(char) * info.size);
        if (files_readFile(filename, fileContent, info.size) != -1) {
            Packet dataPacket = NewPacketFileDataTransfer;
            dataPacket.asFileDataTransferPacket.id = client->downloadedFileId;
            long long sent = 0;
            while (sent < info.size) {
                long long remaining = info.size - sent;
                long long toSend = remaining > FILE_TRANSFER_CHUNK_SIZE ? FILE_TRANSFER_CHUNK_SIZE : remaining;
                memcpy(dataPacket.asFileDataTransferPacket.data, fileContent + sent, toSend);
                if (sendPacket(client->socket, &dataPacket) == -1) {
                    free(fileContent);
                    client->downloadedFileId = 0;
                    destroyThread(&client->downloadThread);
                    return 0;
                }
                sent += toSend;
            }
        } else {
            Packet cancelPacket = NewPacketFileTransferCancel;
            cancelPacket.asFileTransferCancel.id = client->downloadedFileId;
            sendPacket(client->socket, &cancelPacket);
        }

        free(fileContent);
    } else {
        Packet cancelPacket = NewPacketFileTransferCancel;
        cancelPacket.asFileTransferCancel.id = client->downloadedFileId;
        sendPacket(client->socket, &cancelPacket);
    }

    client->downloadedFileId = 0;
    destroyThread(&client->downloadThread);
    return 0;
}

void handleDownloadRequest(Client* client, struct PacketFileDownloadRequest* packet) {
    if (client->downloadedFileId) {
        /* Client is already downloading a file. Refusing download request */

        /* Create packet */
        Packet refuseDownloadPacket = NewPacketFileDownloadValidation;
        struct PacketFileDownloadValidation* validationPacket = &refuseDownloadPacket.asFileDownloadValidationPacket;

        /* Set packet attributes */
        validationPacket->accepted = 0;
        validationPacket->fileId = packet->fileId;

        /* Send packet to client */
        sendPacket(client->socket, &refuseDownloadPacket);
    } else {
        /* Client is not downloading, checking if the requested file is downloadable */

        /* Get file information */
        char filename[12];
        sprintf(filename, "%d", packet->fileId);
        FileInfo fileInfo = files_getInfo(filename);

        if (fileInfo.exists && !fileInfo.isDirectory) {
            /* The requested file can be downloaded */

            /* Create the packet */
            Packet acceptDownloadPacket = NewPacketFileDownloadValidation;
            struct PacketFileDownloadValidation* validationPacket = &acceptDownloadPacket.asFileDownloadValidationPacket;

            /* Set packet attributes */
            validationPacket->accepted = 1;
            validationPacket->fileId = packet->fileId;
            validationPacket->fileSize = fileInfo.size;

            /* Send packet to client */
            sendPacket(client->socket, &acceptDownloadPacket);

            /* Define client download state and start upload worker */
            client->downloadedFileId = packet->fileId;
            client->downloadThread = createThread(uploadFileToClient, client); // Start sending data
        } else {
            /* The requested file can't be downloaded */

            /* Create the packet */
            Packet refuseDownloadPacket = NewPacketFileDownloadValidation;
            struct PacketFileDownloadValidation* validationPacket = &refuseDownloadPacket.asFileDownloadValidationPacket;

            /* Set packet attributes */
            validationPacket->accepted = 0;
            validationPacket->fileId = packet->fileId;

            /* Send packet to client */
            sendPacket(client->socket, &refuseDownloadPacket);
        }
    }
}