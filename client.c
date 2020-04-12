#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sockets.h"

int main() {
    SocketInfo socket = createClientSocket("127.0.0.1", "27015");
    
    const char* message = "Coucou";
    sendTo(socket, message, strlen(message));

    char buffer[250];
    int receivedBytes = receiveFrom(socket, buffer, 249);
    buffer[receivedBytes] = '\0';
    printf("Received %d bytes. Message : %s\n", receivedBytes, buffer);

    closeSocket(socket);

    cleanUp();
    return EXIT_SUCCESS;
}