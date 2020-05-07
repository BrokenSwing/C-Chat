/**
 * \file client.c
 * \brief Client file.
 * 
 * Allows connection to a server and sending messages to another client.
 * 
 */

#include "../common/packets.h"
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "client.h"
#include "commands.h"
#include "../common/constants.h"
#include "../common/sockets.h"
#include "../common/threads.h"
#include "../common/files.h"

static Socket clientSocket;
static char* uploadFilename = NULL;
static Thread uploadThread;

THREAD_ENTRY_POINT sendMessage(void* data) {
    Packet packet = NewPacketText;
    while(1) {
        ui_getUserInput("Your message : ", packet.asTextPacket.message, MSG_MAX_LENGTH);
        if (strncmp("/", packet.asTextPacket.message, 1) == 0) { // Is a command
            commandHandler(packet.asTextPacket.message + 1);
        } else {
            sendPacket(clientSocket, &packet);
        }
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

void sendFile(const char* filename) {
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

COMMAND_HANDLER(command,
COMMAND(file, "Usage: /file <send | receive>",
        COMMAND(send, "Usage: /file send <filepath>",
            if (strlen(command) > 0) {
                    sendFile(command);
                    return;
                }
            )
            COMMAND(receive, "Usage: /file receive <id>",
            if (strlen(command) > 0) {
                    ui_informationMessage("Command receive detected");
                    return;
                }
            )
        )
        COMMAND(nick, "Usage: /nick <username>",
            if (strlen(command) > 0) {
                setUsername(command);
                return;
            }
        )
        COMMAND(quit, "Usage: /quit",
            Packet quitPacket = NewPacketQuit;
            sendPacket(clientSocket, &quitPacket);
            return;
        )
)

void receiveMessages() {
    Packet packet;
    int bytesCount;
    do {
        bytesCount = receiveNextPacket(clientSocket, &packet);
        if (bytesCount > 0) {
            switch (packet.type) {
                case TEXT_MESSAGE_TYPE:
                    ui_messageReceived(packet.asTextPacket.username, packet.asTextPacket.message);
                    break;
                case JOIN_MESSAGE_TYPE:
                    ui_joinMessage(packet.asJoinPacket.username);
                    break;
                case LEAVE_MESSAGE_TYPE:
                    ui_leaveMessage(packet.asLeavePacket.username);
                    break;
                case SERVER_ERROR_MESSAGE_TYPE:
                    ui_errorMessage(packet.asServerErrorMessagePacket.message);
                    break;
                case USERNAME_CHANGED_MESSAGE_TYPE:
                    ; // https://stackoverflow.com/questions/18496282/why-do-i-get-a-label-can-only-be-part-of-a-statement-and-a-declaration-is-not-a
                    char changeMessage[USERNAME_MAX_LENGTH * 2 + 25 + 1];
                    unsigned int oldUsernameLength = strlen(packet.asUsernameChangedPacket.oldUsername);
                    unsigned int newUsernameLength = strlen(packet.asUsernameChangedPacket.newUsername);
                    memcpy(changeMessage, packet.asUsernameChangedPacket.oldUsername, oldUsernameLength);
                    memcpy(changeMessage + oldUsernameLength, " changed its username to ", 25);
                    memcpy(
                            changeMessage + oldUsernameLength + 25,
                            packet.asUsernameChangedPacket.newUsername,
                            newUsernameLength
                    );
                    changeMessage[oldUsernameLength + 25 + newUsernameLength] = '\0';
                    ui_informationMessage(changeMessage);
                    break;
                case FILE_TRANSFER_VALIDATION_MESSAGE_TYPE:
                    if (packet.asFileTransferValidationPacket.accepted) {
                        ui_informationMessage("Beginning file upload.");
                        unsigned int* fileId = malloc(sizeof(unsigned int));
                        *fileId = packet.asFileTransferValidationPacket.id;
                        uploadThread = createThread(fileUploadWorker, fileId);
                    } else {
                        ui_errorMessage("Server rejected file upload.");
                        free(uploadFilename);
                        uploadFilename = NULL;
                    }
                    break;
            }
        }
    } while (bytesCount > 0);
}

void pickUsername() {
    int success = 0;
    do {
        char username[50];
        ui_getUserInput("Your username : ", username, 20);
        int bytesReceived = sendTo(clientSocket, username, strlen(username));

        if (bytesReceived < 0) {
            ui_informationMessage("Connection with server lost. Exiting.");
            closeSocket(&clientSocket);
            cleanUp();
            exit(EXIT_FAILURE);
        }

        bytesReceived = receiveFrom(clientSocket, username, 100);
        if (bytesReceived > 0) {
            username[bytesReceived] = '\0';
            int passwordOk = username[0] == 'O' && username[1] == 'k';
            if (passwordOk) {
                success = 1;
            } else {
                ui_informationMessage(username);
            }
        } else {
            ui_informationMessage("Connection with server lost. Exiting.");
            closeSocket(&clientSocket);
            cleanUp();
            exit(EXIT_FAILURE);
        }
    } while (!success);
}

void setUsername(const char* newUsername) {
    unsigned int userNameLength = strlen(newUsername);
    if (userNameLength > USERNAME_MAX_LENGTH) {
        ui_errorMessage("Username is too long");
        return;
    }

    Packet packet = NewPacketDefineUsername;
    memcpy(packet.asDefineUsernamePacket.username, newUsername, userNameLength + 1);
    sendPacket(clientSocket, &packet);
}

/**
 * \brief Program entry.
 * 
 * \return EXIT_SUCCESS - normal program termination.
 */
int main() {
    ui_init();
    clientSocket = createClientSocket("127.0.0.1", "27015");
    ui_informationMessage("Hi, you're connected to server !");

    pickUsername();
    ui_welcomeMessage();

    Thread senderThread = createThread(sendMessage, NULL);

    receiveMessages();

    destroyThread(&senderThread);

    ui_reset();

    ui_informationMessage("End of chat.");
    ui_cleanUp();

    closeSocket(&clientSocket);
    cleanUp();
    return EXIT_SUCCESS;
}