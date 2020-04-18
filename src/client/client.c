/**
 * \file client.c
 * \brief Client file.
 * 
 * Allows connection to a server and sending messages to another client.
 * 
 */

#include <string.h>
#include <stdlib.h>
#include "../common/sockets.h"
#include <string.h>
#include "ui.h"
#include "../common/threads.h"
#include "../common/constants.h"

static SocketInfo clientSocket;

THREAD_ENTRY_POINT sendMessage(void* data) {
    char buffer[MSG_MAX_LENGTH + 1];
    while(1) {
        ui_getUserInput(buffer, MSG_MAX_LENGTH);
        sendTo(clientSocket, buffer, strlen(buffer));
    }
}

THREAD_ENTRY_POINT receiveMessage(void* data) {
    char buffer[MSG_MAX_LENGTH + USERNAME_MAX_LENGTH + 2];
    int bytesCount;
    do {
        bytesCount = receiveFrom(clientSocket, buffer, MSG_MAX_LENGTH + USERNAME_MAX_LENGTH + 2);
        if (bytesCount != 0) {
            ui_messageReceived(buffer + MSG_MAX_LENGTH + 1, buffer);
        }
    } while (bytesCount != 0);
    return 0;
}

void pickUsername() {
    int success = 0;
    do {
        char username[50];
        ui_getUserInput(username, 20);
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

/**
 * \brief Program entry.
 * 
 * \return EXIT_SUCCESS - normal program termination.
 */
int main() {
    clientSocket = createClientSocket("127.0.0.1", "27015");
    ui_informationMessage("Hi, you're connected to server !");

    pickUsername();

    ui_informationMessage("Joined room.");

    Thread senderThread = createThread(sendMessage, NULL);
    Thread receiverThread = createThread(receiveMessage, NULL);

    joinThread(&receiverThread);
    destroyThread(&senderThread);

    ui_informationMessage("End of chat.");
    closeSocket(&clientSocket);
    cleanUp();
    return EXIT_SUCCESS;
}