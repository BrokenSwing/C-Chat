#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sockets.h"
#include <string.h>

#define BUFFER_SIZE 250

int main() {
    SocketInfo socket = createClientSocket("127.0.0.1", "27015");
    char buffer[BUFFER_SIZE + 1];
    receiveFrom(socket, buffer, BUFFER_SIZE);
    if (buffer[0] == '2') {
        printf("Bonjour Client2\n");
        printf("Que voulez vous envoyer ?\n");
        fgets(buffer, BUFFER_SIZE, stdin);
        *strchr(buffer, '\n') = '\0';
        sendTo(socket, buffer, strlen(buffer) - 1);
    } else {
        printf("Bonjour Client1 veuillez attendre le client 2\n");
    }
    int bytesCount;
    do {
        bytesCount = receiveFrom(socket, buffer, BUFFER_SIZE);
        if (bytesCount != 0) {
            buffer[bytesCount] = '\0';
            printf("Received : %s\n", buffer);
            fgets(buffer, BUFFER_SIZE, stdin);
            *strchr(buffer, '\n') = '\0';
            sendTo(socket, buffer, strlen(buffer) - 1);
        }
    } while (bytesCount != 0);
    printf("End of chat.\n");
    cleanUp();
    return EXIT_SUCCESS;
}
