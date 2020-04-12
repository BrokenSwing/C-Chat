/**
 * \file client.c
 * \brief Client file.
 * 
 * Allows connection to a server and sending messages to another client.
 * 
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sockets.h"
#include <string.h>

/**
 * \def BUFFER_SIZE
 * \brief The buffer size.
 */
#define BUFFER_SIZE 250

/**
 * \brief Get the entered message by the user.
 * 
 * \param buffer Contains the entered message.
*/
void getUserInput(char* buffer) {
    do {
        printf("You : ");
        fgets(buffer, BUFFER_SIZE, stdin);
        if ((strlen(buffer) - 1) == 0) { // If user entered an empty message.
            printf("You can't send an empty message.\n");
        }
    } while((strlen(buffer) - 1) == 0);
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
    printf("Hi, you're connected to server !\n");

    if (buffer[0] == '2') {
        printf("What do you want to send to other connected client ?\n");
        getUserInput(buffer);
        sendTo(socket, buffer, strlen(buffer) - 1); // Don't send carriage return
    } else {
        printf("Wait for an other client to receive a message.\n");
    }

    int bytesCount;
    do {
        bytesCount = receiveFrom(socket, buffer, BUFFER_SIZE);
        if (bytesCount != 0) {
            buffer[bytesCount] = '\0';
            printf("Him : %s\n", buffer);
            getUserInput(buffer);
            sendTo(socket, buffer, strlen(buffer) - 1);
        }
    } while (bytesCount != 0);

    printf("End of chat.\n");
    cleanUp();
    return EXIT_SUCCESS;
}