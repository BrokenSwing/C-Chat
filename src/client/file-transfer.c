#include "file-transfer.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "../common/files.h"

int findFirstFreeUploadIndex() {
    int i = 0;
    while (i < MAX_CONCURRENT_FILE_TRANSFER && uploadData[i].uploadFilename != NULL) {
        i++;
    }

    return i == MAX_CONCURRENT_FILE_TRANSFER ? -1 : i;
}

int findFirstTakenUploadIndex() {
    int i = 0;
    while (i < MAX_CONCURRENT_FILE_TRANSFER && uploadData[i].uploadFilename == NULL) {
        i++;
    }

    return i == MAX_CONCURRENT_FILE_TRANSFER ? -1 : i;
}

int findFirstFreeDownloadIndex() {
    int i = 0;
    while (i < MAX_CONCURRENT_FILE_TRANSFER && downloadData[i].downloadBuffer != NULL) {
        i++;
    }

    return i == MAX_CONCURRENT_FILE_TRANSFER ? -1 : i;
}

int findDownloadIdFor(unsigned int fileId) {
    int i = 0;
    while (i < MAX_CONCURRENT_FILE_TRANSFER && (downloadData[i].downloadBuffer == NULL || downloadData[i].downloadFileId != fileId)) {
        i++;
    }

    return i == MAX_CONCURRENT_FILE_TRANSFER ? -1 : i;
}

void sendFileUploadRequest(const char* filename) {
    int uploadId = findFirstFreeUploadIndex();
    if (uploadId == -1) {
        ui_errorMessage("Can't upload more files.");
        return;
    }

    FileInfo info = files_getInfo(filename);
    if (info.exists && !info.isDirectory) {
        uploadData[uploadId].uploadFilename = malloc(strlen(filename) + 1);
        memcpy(uploadData[uploadId].uploadFilename, filename, strlen(filename) + 1);

        Packet fileUploadPacket = NewPacketFileUploadRequest;
        fileUploadPacket.asFileUploadRequestPacket.fileSize = info.size;
        if(sendPacket(clientSocket, &fileUploadPacket) <= 0) {
            ui_errorMessage("Unable to send the file, unknown error.");
            free(uploadData[uploadId].uploadFilename);
            uploadData[uploadId].uploadFilename = NULL;
        }
    } else {
        ui_errorMessage("This file does not exist or is a directory.");
    }
}

void sendFileDownloadRequest(unsigned int fileId) {
    int downloadId = findDownloadIdFor(fileId);
    if (downloadId == -1) {
        Packet downloadRequestPacket = NewPacketFileDownloadRequest;
        downloadRequestPacket.asFileDownloadRequestPacket.fileId = fileId;
        sendPacket(clientSocket, &downloadRequestPacket);
    } else {
        ui_errorMessage("You're already downloading this file. Just be patient.");
    }
}

void handleFileDownloadCancel(struct PacketFileTransferCancel* packet) {
    int downloadId = findDownloadIdFor(packet->id);
    if (downloadId != -1) {
        free(downloadData[downloadId].downloadBuffer);
        downloadData[downloadId].downloadBuffer = NULL;
        downloadData[downloadId].downloadFileSize = -1;
        downloadData[downloadId].downloadedSize = -1;
        downloadData[downloadId].downloadFileId = 0;
        ui_errorMessage("File download canceled.");
    }
}

THREAD_ENTRY_POINT fileUploadWorker(void* data) {
    unsigned int* threadData = (unsigned int*) data;
    unsigned int fileId = threadData[0];
    unsigned int uploadId = threadData[1];
    free(data);

    FileInfo info = files_getInfo(uploadData[uploadId].uploadFilename);
    char* content = malloc(info.size);
    if (files_readFile(uploadData[uploadId].uploadFilename, content, info.size) != -1) {
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
    free(uploadData[uploadId].uploadFilename);
    uploadData[uploadId].uploadFilename = NULL;
    destroyThread(&uploadData[uploadId].uploadThread);
    return 0;
}

void handleFileUploadValidation(struct PacketFileUploadValidation* packet) {
    int uploadId = findFirstTakenUploadIndex();
    if (uploadId == -1) {
        ui_errorMessage("IMPOSSIBLE ?????:");
        return;
    }

    if (packet->accepted) {
        ui_informationMessage("Beginning file upload.");

        unsigned int* threadData = malloc(sizeof(unsigned int) * 2);
        threadData[0] = packet->id;
        threadData[1] = uploadId;
        uploadData[uploadId].uploadThread = createThread(fileUploadWorker, threadData);
    } else {
        ui_errorMessage("Server rejected file upload.");
        free(uploadData[uploadId].uploadFilename);
        uploadData[uploadId].uploadFilename = NULL;
    }
}

void handleFileData(struct PacketFileDataTransfer* packet) {
    int downloadId = findDownloadIdFor(packet->id);
    if (downloadId != -1) {

        /* Calculating expected next data chunk size */
        unsigned remainingToDownload = downloadData[downloadId].downloadFileSize - downloadData[downloadId].downloadedSize;
        unsigned int nextChunkSize = remainingToDownload > FILE_TRANSFER_CHUNK_SIZE ? FILE_TRANSFER_CHUNK_SIZE : remainingToDownload;

        /* Add received data to file content */
        memcpy(downloadData[downloadId].downloadBuffer + downloadData[downloadId].downloadedSize, packet->data, nextChunkSize);
        downloadData[downloadId].downloadedSize += nextChunkSize;

        if (downloadData[downloadId].downloadedSize >= downloadData[downloadId].downloadFileSize) {
            /* File download is terminated. Writing file content to disk */
            char filename[13];
            sprintf(filename, "r%d", downloadData[downloadId].downloadFileId);
            files_writeFile(filename, downloadData[downloadId].downloadBuffer, downloadData[downloadId].downloadFileSize);

            /* Reset download state */
            free(downloadData[downloadId].downloadBuffer);
            downloadData[downloadId].downloadBuffer = NULL;
            downloadData[downloadId].downloadFileSize = -1;
            downloadData[downloadId].downloadedSize = -1;
            downloadData[downloadId].downloadFileId = 0;
            ui_informationMessage("File download complete");
        }
    }
}

void handleFileDownloadValidation(struct PacketFileDownloadValidation* packet) {
    int downloadId = findFirstFreeDownloadIndex();
    if (packet->accepted && downloadId != -1) {
        downloadData[downloadId].downloadFileSize = packet->fileSize;
        downloadData[downloadId].downloadBuffer = malloc(sizeof(char) * downloadData[downloadId].downloadFileSize);
        downloadData[downloadId].downloadedSize = 0;
        downloadData[downloadId].downloadFileId = packet->fileId;
        ui_informationMessage("File download beginning.");
    } else {
        ui_errorMessage("Server rejected file download.");
    }
}