/**
 * \file server.c
 * \brief Serveur file.
 * 
 * Allows connection of two clients. Then transmits messages form one client to another client.
 * 
 */

#include "../common/sockets.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

/**
 * \def MSG_MAX_LENGTH
 * \brief Maximum message size.
 */
#define MSG_MAX_LENGTH 250

#define NUMBER_CLIENT_MAX 2

SocketInfo clientSocket[NUMBER_CLIENT_MAX];

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

THREAD_ENTRY_POINT Message(void* data) {
    char buffer[MSG_MAX_LENGTH + 1];
    int bytesCount;
    do {

        bytesCount = receiveFrom(clientSocket[(int)data], buffer, MSG_MAX_LENGTH);
        if (bytesCount == 0) { // Connection with sender is lost
            closeSocket(clientSocket[(int)data]);
            printf("Connection interrupted by sender.\n");
            break;
        }
        buffer[bytesCount] = '\0';

        if (receivedEndMessage(buffer)) {
            printf("A client want to leave. Closing sockets.\n");
            for(int i = 0; i < NUMBER_CLIENT_MAX; i++) {
                closeSocket(clientSocket[i]);
            }
            break;
        }

        for(int i = 0; i < NUMBER_CLIENT_MAX; i++) {
            if(i != (int)data) {
                bytesCount = sendTo(clientSocket[i], buffer, bytesCount);
                if (bytesCount == 0) { // Connection with receiver is lost
                    closeSocket(clientSocket[(int)data]);
                    printf("Connection interrupted by receiver.\n");
                    break;
                }
            }
        }

    } while (1);
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
        clientSocket[0] = acceptClient(serverSocket);
        sendTo(clientSocket[0], "1", 1);
        printf("Waiting for second client to connect.\n");
        clientSocket[1] = acceptClient(serverSocket);
        sendTo(clientSocket[1], "2", 1);

        printf("Two clients connected !\n");

        do {

            Thread OneToTwo = createThread(Message);
            Thread TwoToOne = createThread(Message);

            joinThread(OneToTwo);
            joinThread(TwoToOne);

        } while(1);
    } while(1);
}