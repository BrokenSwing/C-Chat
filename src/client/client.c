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

/**
 * \def BUFFER_SIZE
 * \brief The buffer size.
 */
#define BUFFER_SIZE 250

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

    if (buffer[0] == '2') {
        ui_informationMessage("What do you want to send to other connected client ?");
        ui_getUserInput(buffer, BUFFER_SIZE);
        sendTo(socket, buffer, strlen(buffer));
    } else {
        ui_informationMessage("Wait for an other client to receive a message.");
    }

    int bytesCount;
    do {
        bytesCount = receiveFrom(socket, buffer, BUFFER_SIZE);
        if (bytesCount != 0) {
            buffer[bytesCount] = '\0';
            ui_messageReceived("Him", buffer);
            ui_getUserInput(buffer, BUFFER_SIZE);
            sendTo(socket, buffer, strlen(buffer));
        }
    } while (bytesCount != 0);

    ui_informationMessage("End of chat.");
    cleanUp();
    return EXIT_SUCCESS;
}