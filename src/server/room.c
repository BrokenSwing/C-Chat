#include "room.h"
#include <stdlib.h>
#include <string.h>
#include "communication.h"

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
    } else if (strlen(packet->roomName) == 0) {
        Packet errorPacket = NewPacketServerErrorMessage;
        memcpy(errorPacket.asServerErrorMessagePacket.message, "The room name can't be empty.", 30);
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
        for (int i = 1; i < MAX_USERS_PER_ROOM; i++) {
            room->clients[i] = NULL;
        }
        client->room = room;
        rooms[roomId] = room;

        Packet successPacket = NewPacketServerSuccess;
        memcpy(successPacket.asServerSuccessMessagePacket.message, "Room created !", 15);
        sendPacket(client->socket, &successPacket);
    }
}

void handleRoomJoinRequest(Client* client, struct PacketJoinRoom* packet) {
    Room* room = findRoomByName(packet->roomName);
    if (room == NULL) {
        Packet errorPacket = NewPacketServerErrorMessage;
        memcpy(errorPacket.asServerErrorMessagePacket.message, "This room does not exist.", 26);
        sendPacket(client->socket, &errorPacket);
        return;
    }

    if (client->room == room) {
        Packet errorPacket = NewPacketServerErrorMessage;
        memcpy(errorPacket.asServerErrorMessagePacket.message, "You're already in this room.", 29);
        sendPacket(client->socket, &errorPacket);
        return;
    }

    int slot = findFirstFreeSlotForRoom(room);
    if (slot == -1) {
        Packet errorPacket = NewPacketServerErrorMessage;
        memcpy(errorPacket.asServerErrorMessagePacket.message, "This room is full.", 19);
        sendPacket(client->socket, &errorPacket);
        return;
    }

    // TODO: Make user quit a room if already in one
    room->clients[slot] = client;
    client->room = room;

    Packet joinPacket = NewPacketJoin;
    SYNC_CLIENT_READ(memcpy(joinPacket.asJoinPacket.username, client->username, USERNAME_MAX_LENGTH + 1));

    broadcastRoom(&joinPacket, room);
}