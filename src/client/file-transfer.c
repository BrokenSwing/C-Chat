#include "file-transfer.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "../common/files.h"

void sendFileUploadRequest(const char* filename) {
    if (uploadFilename) {
        ui_errorMessage("Already uploading a file.");
        return;
    }

    FileInfo info = files_getInfo(filename);
    if (info.exists && !info.isDirectory) {
        uploadFilename = malloc(strlen(filename) + 1);
        memcpy(uploadFilename, filename, strlen(filename) + 1);

        Packet fileUploadPacket = NewPacketFileUploadRequest;
        fileUploadPacket.asFileUploadRequestPacket.fileSize = info.size;
        if(sendPacket(clientSocket, &fileUploadPacket) <= 0) {
            ui_errorMessage("Unable to send the file, unknown error.");
            free(uploadFilename);
            uploadFilename = NULL;
        }
    } else {
        ui_errorMessage("This file does not exist or is a directory.");
    }
}

void sendFileDownloadRequest(unsigned int fileId) {
    Packet downloadRequestPacket = NewPacketFileDownloadRequest;
    downloadRequestPacket.asFileDownloadRequestPacket.fileId = fileId;
    sendPacket(clientSocket, &downloadRequestPacket);
}

void handleFileDownloadCancel(struct PacketFileTransferCancel* packet) {
    if (downloadBuffer != NULL && packet->id == downloadFileId) {
        free(downloadBuffer);
        downloadBuffer = NULL;
        downloadFileSize = -1;
        downloadedSize = -1;
        downloadFileId = 0;
        ui_errorMessage("File download canceled.");
    }
}

THREAD_ENTRY_POINT fileUploadWorker(void* data) {
    unsigned int fileId = *((unsigned int*)data);
    free(data);

    FileInfo info = files_getInfo(uploadFilename);
    char* content = malloc(info.size);
    if (files_readFile(uploadFilename, content, info.size) != -1) {
        Packet dataPacket = NewPacketFileDataTransfer;
        dataPacket.asFileDataTransferPacket.id = fileId;
        long long sent = 0;
        while (sent < info.size) {
            long long remaining = info.size - sent;
            long long toSend = remaining > FILE_TRANSFER_CHUNK_SIZE ? FILE_TRANSFER_CHUNK_SIZE : remaining;
            memcpy(dataPacket.asFileDataTransferPacket.data, content + sent, toSend);
            sendPacket(clientSocket, &dataPacket);
            sent += toSend;
        }
    } else {
        ui_errorMessage("Unable to read file content.");
    }
    free(content);
    free(uploadFilename);
    uploadFilename = NULL;
    destroyThread(&uploadThread);
    return 0;
}

void handleFileUploadValidation(struct PacketFileUploadValidation* packet) {
    if (packet->accepted) {
        ui_informationMessage("Beginning file upload.");
        unsigned int* fileId = malloc(sizeof(unsigned int));
        *fileId = packet->id;
        uploadThread = createThread(fileUploadWorker, fileId);
    } else {
        ui_errorMessage("Server rejected file upload.");
        free(uploadFilename);
        uploadFilename = NULL;
    }
}

void handleFileData(struct PacketFileDataTransfer* packet) {
    if (downloadBuffer && packet->id == downloadFileId) {

        /* Calculating expected next data chunk size */
        unsigned remainingToDownload = downloadFileSize - downloadedSize;
        unsigned int nextChunkSize = remainingToDownload > FILE_TRANSFER_CHUNK_SIZE ? FILE_TRANSFER_CHUNK_SIZE : remainingToDownload;

        /* Add received data to file content */
        memcpy(downloadBuffer + downloadedSize, packet->data, nextChunkSize);
        downloadedSize += nextChunkSize;

        if (downloadedSize >= downloadFileSize) {
            /* File download is terminated. Writing file content to disk */
            char filename[13];
            sprintf(filename, "r%d", downloadFileId);
            files_writeFile(filename, downloadBuffer, downloadFileSize);

            /* Reset download state */
            free(downloadBuffer);
            downloadBuffer = NULL;
            downloadFileSize = -1;
            downloadedSize = -1;
            downloadFileId = 0;
            ui_informationMessage("File download complete");
        }
    }
}

void handleFileDownloadValidation(struct PacketFileDownloadValidation* packet) {
    if (packet->accepted) {
        downloadFileSize = packet->fileSize;
        downloadBuffer = malloc(sizeof(char) * downloadFileSize);
        downloadedSize = 0;
        downloadFileId = packet->fileId;
        ui_informationMessage("File download beginning.");
    } else {
        ui_errorMessage("Server rejected file download.");
    }
}