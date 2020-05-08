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
        Packet response = NewPacketFileUploadValidation;
        response.asFileUploadValidationPacket.accepted = 0;
        response.asFileUploadValidationPacket.id = 0;
        sendPacket(client->socket, &response);

        response = NewPacketServerErrorMessage;
        memcpy(response.asServerErrorMessagePacket.message, "Already uploading.", 19);
        sendPacket(client->socket, &response);
    } else if (packet->fileSize <= 0 || packet->fileSize > MAX_FILE_SIZE_UPLOAD) {
        Packet response = NewPacketFileUploadValidation;
        response.asFileUploadValidationPacket.accepted = 0;
        response.asFileUploadValidationPacket.id = 0;
        sendPacket(client->socket, &response);

        response = NewPacketServerErrorMessage;
        memcpy(response.asServerErrorMessagePacket.message, "File too large.", 16);
        sendPacket(client->socket, &response);

    } else {
        unsigned int fileId = generateNewFileId();

        Packet response = NewPacketFileUploadValidation;
        response.asFileUploadValidationPacket.accepted = 1;
        response.asFileUploadValidationPacket.id = fileId;
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

THREAD_ENTRY_POINT uploadFileToClient(void* data) {
    // TODO: Add packet that cancels a download
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
                sendPacket(client->socket, &dataPacket); // TODO: Check sent size and cancel download in case of error
                sent += toSend;
            }
            Packet downloadSuccessPacket = NewPacketServerErrorMessage; // TODO: Replace with ServerInformationPacket
            memcpy(downloadSuccessPacket.asServerErrorMessagePacket.message, "File downloaded.", 17);
            sendPacket(client->socket, &downloadSuccessPacket);
        } else {
            // Cancel download
        }

        free(fileContent);
    } else {
        // Cancel download
    }

    client->downloadedFileId = 0;
    destroyThread(&client->downloadThread);
    return 0;
}

void handleDownloadRequest(Client* client, struct PacketFileDownloadRequest* packet) {
    if (client->downloadedFileId) {
        Packet refuseDownloadPacket = NewPacketFileDownloadValidation;
        refuseDownloadPacket.asFileDownloadValidationPacket.accepted = 0;
        refuseDownloadPacket.asFileDownloadValidationPacket.fileId = packet->fileId;
        sendPacket(client->socket, &refuseDownloadPacket);
    } else {
        char filename[12];
        sprintf(filename, "%d", packet->fileId);
        FileInfo fileInfo = files_getInfo(filename);
        if (fileInfo.exists && !fileInfo.isDirectory) {
            Packet acceptDownloadPacket = NewPacketFileDownloadValidation;
            acceptDownloadPacket.asFileDownloadValidationPacket.accepted = 1;
            acceptDownloadPacket.asFileDownloadValidationPacket.fileId = packet->fileId;
            acceptDownloadPacket.asFileDownloadValidationPacket.fileSize = fileInfo.size;

            sendPacket(client->socket, &acceptDownloadPacket);

            client->downloadedFileId = packet->fileId;
            client->downloadThread = createThread(uploadFileToClient, client); // Start sending data
        } else {
            Packet refuseDownloadPacket = NewPacketFileDownloadValidation;
            refuseDownloadPacket.asFileDownloadValidationPacket.accepted = 0;
            refuseDownloadPacket.asFileDownloadValidationPacket.fileId = packet->fileId;
            sendPacket(client->socket, &refuseDownloadPacket);
        }
    }
}