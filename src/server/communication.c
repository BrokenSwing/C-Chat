#include "communication.h"
#include "client-info.h"
#include <stdlib.h>
#include <string.h>

void broadcast(Packet* packet) {
    SYNC_CLIENT_READ(
        for (int i = 0; i < NUMBER_CLIENT_MAX; i++) {
            Client *c = clients[i];
            if (c != NULL && c->joined) {
                sendPacket(c->socket, packet);
            }
        }
    )
}

void handleTextMessageRelay(Client* client, struct PacketText* packet) {
    unsigned int messageLength = strlen(packet->message);
    if (messageLength > 0 && messageLength <= MSG_MAX_LENGTH) {
        getClientUsername(client, packet->username);
        broadcast( (Packet*) packet);
    } else {
        Packet serverErrorPacket = NewPacketServerErrorMessage;
        memcpy(serverErrorPacket.asServerErrorMessagePacket.message, "Invalid message.", 17);
        sendPacket(client->socket, &serverErrorPacket);
    }
}