/**
 * \file server.c
 * \brief Serveur file.
 * 
 * Allows connection of two clients. Then transmits messages form one client to another client.
 * 
 */

#include "sockets.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

/**
 * \def MSG_MAX_LENGTH
 * \brief Maximum message size.
 */
#define MSG_MAX_LENGTH 250

/**
 * \brief Catch interrupt signal.
 * 
 * \param signal Incoming signal.
 */
void handleServerClose(int signal) {
    cleanUp();
    printf("Server closed.\n");
    exit(EXIT_SUCCESS);
}

/**
 * \brief Verifies if the end of the communication is requested.
 * 
 * \param buffer A message.
 * \return 1 if the message is "fin", else 0.
 */
int receivedEndMessage(const char* buffer) {
    return buffer[0] == 'f' && buffer[1] == 'i' && buffer[2] == 'n' && buffer[3] == '\0';
}

/**
 * \brief Program entry.
 * 
 * \return EXIT_SUCCESS - normal program termination.
 */
int main () {
    SocketInfo serverSocket = createServerSocket("27015");
    signal(SIGINT, handleServerClose);

    char buffer[MSG_MAX_LENGTH + 1];

    do {
        printf("Waiting for first client to connect.\n");
        SocketInfo receiver = acceptClient(serverSocket);
        sendTo(receiver, "1", 1);
        printf("Waiting for second client to connect.\n");
        SocketInfo sender = acceptClient(serverSocket);
        sendTo(sender, "2", 1);

        printf("Two clients connected !\n");

        do {
            int bytesCount = receiveFrom(sender, buffer, MSG_MAX_LENGTH);
            if (bytesCount == 0) { // Connection with sender is lost
                closeSocket(receiver);
                printf("Connection interrupted by sender.\n");
                break;
            }
            buffer[bytesCount] = '\0';

            if (receivedEndMessage(buffer)) {
                printf("A client want to leave. Closing sockets.\n");
                closeSocket(sender);
                closeSocket(receiver);
                break;
            }

            bytesCount = sendTo(receiver, buffer, bytesCount);
            if (bytesCount == 0) { // Connection with receiver is lost
                closeSocket(sender);
                printf("Connection interrupted by receiver.\n");
                break;
            }

            printf("Relayed message : %s\n", buffer);

            SocketInfo tmp = sender;
            sender = receiver;
            receiver = tmp;

        } while(1);
    } while(1);
}