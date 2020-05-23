#include "room.h"
#include <stdlib.h>
#include <string.h>

int findFirstFreeRoomSlot() {
    int i = 0;
    while (i < NUMBER_ROOM_MAX && rooms[i] != NULL) {
        i++;
    }
    return i == NUMBER_ROOM_MAX ? -1 : i;
}

int findFirstFreeSlotForRoom(Room* room) {
    int i = 0;
    while (i < MAX_USERS_PER_ROOM && room->clients[i] != NULL) {
        i++;
    }

    return i == MAX_USERS_PER_ROOM ? -1 : i;
}

Room* findRoomByName(const char* roomName) {
    int i = 0;
    while (i < NUMBER_ROOM_MAX && (rooms[i] == NULL || strcmp(rooms[i]->name, roomName) != 0)) {
        i++;
    }

    return i == NUMBER_ROOM_MAX ? NULL : rooms[i];
}

void handleRoomCreationRequest(Client* client, struct PacketCreateRoom* packet) {
    if (client->room != NULL) {
        Packet errorPacket = NewPacketServerErrorMessage;
        memcpy(errorPacket.asServerErrorMessagePacket.message, "You're already in a room. First leave the room.", 48);
        sendPacket(client->socket, &errorPacket);
    } else {
        Room* room = findRoomByName(packet->roomName);

        /* Name already taken */
        if (room != NULL) {
            Packet errorPacket = NewPacketServerErrorMessage;
            memcpy(errorPacket.asServerErrorMessagePacket.message, "This room name is already used.", 32);
            sendPacket(client->socket, &errorPacket);
            return;
        }

        int roomId = findFirstFreeRoomSlot();

        /* No room slot available */
        if (roomId == -1) {
            Packet errorPacket = NewPacketServerErrorMessage;
            memcpy(errorPacket.asServerErrorMessagePacket.message, "The maximum amount of rooms is reached.", 40);
            sendPacket(client->socket, &errorPacket);
            return;
        }

        room = malloc(sizeof(Room));
        memcpy(room->name, packet->roomName, ROOM_NAME_MAX_LENGTH + 1);
        memcpy(room->description, packet->roomDesc, ROOM_DESC_MAX_LENGTH + 1);
        room->owner = client;
        room->clients[0] = client;
        client->room = room;
        rooms[roomId] = room;

        Packet successPacket = NewPacketServerSuccess;
        memcpy(successPacket.asServerSuccessMessagePacket.message, "Room created !", 15);
        sendPacket(client->socket, &successPacket);
    }
}