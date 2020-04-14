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
#include ""

/**
 * \def BUFFER_SIZE
 * \brief The buffer size.
 */
#define BUFFER_SIZE 250

static SocketInfo socket;

THREAD_ENTRY_POINT sendMessage(void* data) {
    char buffer[BUFFER_SIZE + 1];
    while(1) {
        ui_getUserInput(buffer, BUFFER_SIZE);
        sendTo(socket, buffer, strlen(buffer));
    }
}

THREAD_ENTRY_POINT receiveMessage(void* data) {
    char buffer[BUFFER_SIZE + 1];
    int bytesCount;
    do {
        bytesCount = receiveFrom(socket, buffer, BUFFER_SIZE);
        if (bytesCount != 0) {
            buffer[bytesCount] = '\0';
            ui_messageReceived("Him", buffer);
        }
    } while (bytesCount != 0);
}

/**
 * \brief Program entry.
 * 
 * \return EXIT_SUCCESS - normal program termination.
 */
int main() {
    char buffer[BUFFER_SIZE + 1];
    SocketInfo socket = createClientSocket("127.0.0.1", "27015");

    receiveFrom(socket, buffer, BUFFER_SIZE);
    ui_informationMessage("Hi, you're connected to server !");

    Thread senderThread = createThread(sendMessage);
    Thread receiverThread = createThread(receiveMessage);

    joinThread(receiverThread);
    destroyThread(senderThread);

    ui_informationMessage("End of chat.");
    cleanUp();
    return EXIT_SUCCESS;
}