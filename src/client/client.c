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
#include "../common/files.h"
#include <string.h>
#include <stdio.h>
#include "file-transfer.h"
#include "room.h"

Socket clientSocket;
struct UploadData uploadData[MAX_CONCURRENT_FILE_TRANSFER];
struct DownloadData downloadData[MAX_CONCURRENT_FILE_TRANSFER];

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

COMMAND_HANDLER(command,
COMMAND(file, "Usage: /file <send | receive>",
        COMMAND(send, "Usage: /file send <filepath>",
                if (strlen(command) > 0) {
                    sendFileUploadRequest(command);
                    return;
                }
            )
            COMMAND(receive, "Usage: /file receive <id>",
                if (strlen(command) > 0) {
                    long id = strtol(command, NULL, 10);
                    if (id > 0) {
                        sendFileDownloadRequest(id);
                        return;
                    }
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
        COMMAND(room, "Usage: /room <create | join | leave | list>",
            COMMAND(create, "Usage: /room create <name> <description>",
                    if(strlen(command) > 0) {
                        createRoom(command);
                        return;
                    }
                )
                COMMAND(join, "Usage: /room join <name>",
                    if (strlen(command) > 0) {
                        joinRoom(command);
                        return;
                    }
                )
                COMMAND(leave, "Usage: /room leave",
                    leaveRoom();
                    return;
                )
                COMMAND(list, "Usage: /room list",
                    listRooms();
                    return;
                )
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
                    ui_usernameChanged(packet.asUsernameChangedPacket.oldUsername, packet.asUsernameChangedPacket.newUsername);
                    break;
                case FILE_UPLOAD_VALIDATION_MESSAGE_TYPE:
                    handleFileUploadValidation(&packet.asFileUploadValidationPacket);
                    break;
                case FILE_DOWNLOAD_VALIDATION_MESSAGE_TYPE:
                    handleFileDownloadValidation(&packet.asFileDownloadValidationPacket);
                    break;
                case FILE_DATA_TRANSFER_MESSAGE_TYPE:
                    handleFileData(&packet.asFileDataTransferPacket);
                    break;
                case FILE_TRANSFER_CANCEL_MESSAGE_TYPE:
                    handleFileDownloadCancel(&packet.asFileTransferCancelPacket);
                    break;
                case SERVER_SUCCESS_MESSAGE_TYPE:
                    ui_successMessage(packet.asServerSuccessMessagePacket.message);
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
    /* Initialize upload data */
    for (int i = 0; i < MAX_CONCURRENT_FILE_TRANSFER; i++) {
        uploadData[i].uploadFilename = NULL;
        downloadData[i].downloadBuffer = NULL;
        downloadData[i].downloadFileId = 0;
    }

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