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

void broadcastRoom(Packet* packet, Room* room) {
    SYNC_CLIENT_READ(
        for (int i = 0; i < MAX_USERS_PER_ROOM; i++) {
            if (room->clients[i] != NULL) {
                sendPacket(room->clients[i]->socket, packet);
            }
        }
    )
}

void handleTextMessageRelay(Client* client, struct PacketText* packet) {
    if (client->room == NULL) {
        Packet errorPacket = NewPacketServerErrorMessage;
        memcpy(errorPacket.asServerErrorMessagePacket.message, "First join a room using /room join <name>.", 43);
        sendPacket(client->socket, &errorPacket);
        return;
    }

    unsigned int messageLength = strlen(packet->message);
    if (messageLength > 0 && messageLength <= MSG_MAX_LENGTH) {
        getClientUsername(client, packet->username);
        broadcastRoom( (Packet*) packet, client->room);
    } else {
        Packet serverErrorPacket = NewPacketServerErrorMessage;
        memcpy(serverErrorPacket.asServerErrorMessagePacket.message, "Invalid message.", 17);
        sendPacket(client->socket, &serverErrorPacket);
    }
}