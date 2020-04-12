#include "sockets.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main () {
    SocketInfo serverSocket = createServerSocket("27015");

    SocketInfo clientSocket = createClient("ip", "port");

    int numClient;

    res = receiveFrom(clientSocket, buffer, 249)

   if (res[0] == '1') {
   //attendre le 2ème client
    printf("%s\n", "Bonjour, vous êtes connecté en tant que client 1, veuillez attendre le message du client 2");
    while (1) {
    //reception du message de 2
        char msg2[249];
        res = receiveFrom(clientSocket, &msg2, 249);
        if (res = 0) {
            printf('La communication est terminée \n');
            closeSocket(clientSocket);
        }
        else {
            printf("Le client 2 a dit : %s\n" , msg2);
        }
    //envoie du message à 2
        char msg[249];
        printf("Que voulez vous envoyer ? \n");
        fgets(msg, 249, stdin);
        *strchr(msg, '\n') = '\0';
        sendTo(clientSocket, &msg, strlen(msg))
    }

   }

   else if (res[0] == '2') == 0) {
    printf(%s\n, "Bonjour Client2")
    //envoie du message à 1
    char msg[249];
    printf("Que voulez vous envoyer ?\n");
    fgets(msg, 249, stdin);
    *strchr(msg, '\n') = '\0'
    sendTo(clientSocket, $msg, strlen(msg))
    do {
    //réception du message de 1
        char msg1[249];
        res = receiveFrom(clientSocket, $msg1, 249);
        if (res == 0) {
            printf('La communication est terminée \n');
            closeSocket(clientSocket);
        }
        else {
            printf("Le client 1 a dit : %s\n", msg1);
        }
   } while (res != 0)

cleanUp();
return EXIT_SUCCESS;
}
