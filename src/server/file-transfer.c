#include "file-transfer.h"
#include <stdlib.h>
#include "../common/files.h"
#include <stdio.h>
#include "communication.h"
#include "string.h"

static unsigned int nextFileId = 1; // TODO: synchronize access

// Simple implementation
unsigned int generateNewFileId() {
    return nextFileId++;
}

int findAvailableUploadSlot(Client* client) {
    int i = 0;
    while (i < MAX_CONCURRENT_FILE_TRANSFER && client->uploadData[i].fileId != 0) {
        i++;
    }
    return i == MAX_CONCURRENT_FILE_TRANSFER ? -1 : i;
}

void handleUploadRequest(Client* client, struct PacketFileUploadRequest* packet) {
    int uploadId = findAvailableUploadSlot(client);
    int fileTooLarge = packet->fileSize <= 0 || packet->fileSize > MAX_FILE_SIZE_UPLOAD;
    if (uploadId == -1 || fileTooLarge) {
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
        if (uploadId == -1) {
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
        client->uploadData[uploadId].fileId = fileId;
        client->uploadData[uploadId].fileContent = malloc(sizeof(char) * packet->fileSize);
        client->uploadData[uploadId].fileSize = packet->fileSize;
        client->uploadData[uploadId].received = 0;
    }
}

int findUploadIdForFile(Client* client, unsigned fileId) {
    int i = 0;
    while (i < MAX_CONCURRENT_FILE_TRANSFER && client->uploadData[i].fileId != fileId) {
        i++;
    }

    return i == MAX_CONCURRENT_FILE_TRANSFER ? -1 : i;
}

void handleFileDataUpload(Client* client, struct PacketFileDataTransfer* packet) {
    int uploadId = findUploadIdForFile(client, packet->id);
    if (packet->id > 0 && uploadId != -1) {
        /* The client is uploading and the packet data refers to the current upload file */

        /* Calculating expected data chunk size */
        unsigned remainingToDownload = client->uploadData[uploadId].fileSize - client->uploadData[uploadId].received;
        unsigned int nextChunkSize = remainingToDownload > FILE_TRANSFER_CHUNK_SIZE ? FILE_TRANSFER_CHUNK_SIZE : remainingToDownload;

        /* Appending data to file content */
        memcpy(client->uploadData[uploadId].fileContent + client->uploadData[uploadId].received, packet->data, nextChunkSize);
        client->uploadData[uploadId].received += nextChunkSize;

        if (client->uploadData[uploadId].received >= client->uploadData[uploadId].fileSize) {
            /* We received all file content */

            /* Writing file to disk */
            char filename[12];
            sprintf(filename, "%d", client->uploadData[uploadId].fileId);
            files_writeFile(filename, client->uploadData[uploadId].fileContent, client->uploadData[uploadId].fileSize);

            /* Telling clients a new file is available */
            Packet uploadSuccessPacket = NewPacketServerErrorMessage; // TODO: Create a ServerInformation packet
            sprintf(
                uploadSuccessPacket.asServerErrorMessagePacket.message,
                "%s uploaded file %d",
                client->username,
                client->uploadData[uploadId].fileId
            );
            broadcast(&uploadSuccessPacket);

            /* Set client upload state */
            free(client->uploadData[uploadId].fileContent);
            client->uploadData[uploadId].fileId = 0;
            client->uploadData[uploadId].received = 0;
            client->uploadData[uploadId].fileContent = NULL;
        }
    } // Just ignoring packet if id does not match
}

int findAvailableDownloadSlot(Client* client) {
    int i = 0;
    while(i < MAX_CONCURRENT_FILE_TRANSFER && client->downloadData[i].downloadedFileId != 0) {
        i++;
    }

    return i == MAX_CONCURRENT_FILE_TRANSFER ? -1 : i;
}

THREAD_ENTRY_POINT uploadFileToClient(void* data) {
    Client* client = *((Client**)data);
    int downloadId = *((int*)data + sizeof(Client*));

    char filename[12];
    sprintf(filename, "%d", client->downloadData[downloadId].downloadedFileId);
    FileInfo info = files_getInfo(filename);

    if (info.exists && !info.isDirectory) {
        char* fileContent = malloc(sizeof(char) * info.size);
        if (files_readFile(filename, fileContent, info.size) != -1) {
            Packet dataPacket = NewPacketFileDataTransfer;
            dataPacket.asFileDataTransferPacket.id = client->downloadData[downloadId].downloadedFileId;
            long long sent = 0;
            while (sent < info.size) {
                long long remaining = info.size - sent;
                long long toSend = remaining > FILE_TRANSFER_CHUNK_SIZE ? FILE_TRANSFER_CHUNK_SIZE : remaining;
                memcpy(dataPacket.asFileDataTransferPacket.data, fileContent + sent, toSend);
                if (sendPacket(client->socket, &dataPacket) == -1) {
                    free(fileContent);
                    client->downloadData[downloadId].downloadedFileId = 0;
                    destroyThread(&client->downloadData[downloadId].downloadThread);
                    return 0;
                }
                sent += toSend;
            }
        } else {
            Packet cancelPacket = NewPacketFileTransferCancel;
            cancelPacket.asFileTransferCancel.id = client->downloadData[downloadId].downloadedFileId;
            sendPacket(client->socket, &cancelPacket);
        }

        free(fileContent);
    } else {
        Packet cancelPacket = NewPacketFileTransferCancel;
        cancelPacket.asFileTransferCancel.id = client->downloadData[downloadId].downloadedFileId;
        sendPacket(client->socket, &cancelPacket);
    }

    client->downloadData[downloadId].downloadedFileId = 0;
    destroyThread(&client->downloadData[downloadId].downloadThread);
    return 0;
}

void handleDownloadRequest(Client* client, struct PacketFileDownloadRequest* packet) {
    int downloadId = findAvailableDownloadSlot(client);
    if (downloadId == -1) {
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

            /* Allocating thread data */
            void* threadData = malloc(sizeof(Client*) + sizeof(int));
            *((Client**)threadData) = client;
            *((int*)threadData + sizeof(Client*)) = downloadId;

            /* Define client download state and start upload worker */
            client->downloadData[downloadId].downloadedFileId = packet->fileId;
            client->downloadData[downloadId].downloadThread = createThread(uploadFileToClient, threadData); // Start sending data
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