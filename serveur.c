#include "sockets.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main () {
    SocketInfo serverSocket = createServerSocket("27015");

    SocketInfo clientSocket = acceptClient(serverSocket);

    char buffer[250];
    int bytesReceived = receiveFrom(clientSocket, buffer, 249);
    buffer[bytesReceived] = '\0';
    printf("Received %d bytes. Message : %s\n", bytesReceived, buffer);

    const char* answer = "Got it!";
    sendTo(clientSocket, answer, strlen(answer));

    closeSocket(clientSocket);

    closeSocket(serverSocket);
    cleanUp();
    return EXIT_SUCCESS;
}