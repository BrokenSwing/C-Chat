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
    for (int i = 0; i < MAX_USERS_PER_ROOM; i++) {
        if (room->clients[i] != NULL) {
            sendPacket(room->clients[i]->socket, packet);
        }
    }
}

void syncBroadcastRoom(Packet* packet, Room* room) {
    SYNC_ROOM_READ(room, broadcastRoom(packet, room));
}

void handleTextMessageRelay(Client* client, struct PacketText* packet) {
    unsigned int messageLength = strlen(packet->message);

    if (messageLength > 0 && messageLength <= MSG_MAX_LENGTH) {
        getClientUsername(client, packet->username);

        // Allow to be sure client's room don't change between NULL check and room-lock acquire
        acquireRead(clientsLock);

        Room* room = client->room;
        if (room == NULL) {
            releaseRead(clientsLock);
            Packet errorPacket = NewPacketServerErrorMessage;
            memcpy(errorPacket.asServerErrorMessagePacket.message, "First join a room using /room join <name>.", 43);
            sendPacket(client->socket, &errorPacket);
            return;
        }

        acquireRead(room->lock);
        releaseRead(clientsLock);

        broadcastRoom( (Packet*) packet, room);
        releaseRead(room->lock);
    } else {
        Packet serverErrorPacket = NewPacketServerErrorMessage;
        memcpy(serverErrorPacket.asServerErrorMessagePacket.message, "Invalid message.", 17);
        sendPacket(client->socket, &serverErrorPacket);
    }
}