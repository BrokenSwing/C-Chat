#include "room.h"
#include <stdlib.h>
#include <string.h>
#include "communication.h"
#include <stdio.h>
#include "client-info.h"

int findFirstFreeRoomSlot() {
    int i = 0;
    while (i < NUMBER_ROOM_MAX && rooms[i] != NULL) {
        i++;
    }
    return i == NUMBER_ROOM_MAX ? -1 : i;
}

int findFirstFreeSlotForRoom(Room *room) {
    int i = 0;
    while (i < MAX_USERS_PER_ROOM && room->clients[i] != NULL) {
        i++;
    }

    return i == MAX_USERS_PER_ROOM ? -1 : i;
}

Room *findRoomByName(const char *roomName) {
    int i = 0;
    while (i < NUMBER_ROOM_MAX && (rooms[i] == NULL || strcmp(rooms[i]->name, roomName) != 0)) {
        i++;
    }

    return i == NUMBER_ROOM_MAX ? NULL : rooms[i];
}

int findRoomId(Room *room) {
    int i = 0;
    while (i < NUMBER_ROOM_MAX && rooms[i] != room) {
        i++;
    }

    return i == NUMBER_ROOM_MAX ? -1 : i;
}

/**
 * \brief Creates a room with the given name, the given description and the given owner.
 *
 * It initializes all room fields and add the owner to the clients connected to the room.
 *
 * \param owner The owner of the room
 * \param name The name of the room
 * \param description The description of the room
 * \return the newly created room
 */
Room *createRoom(Client *owner, const char *name, const char *description) {
    Room *room = malloc(sizeof(Room));
    memcpy(room->name, name, ROOM_NAME_MAX_LENGTH + 1);
    memcpy(room->description, description, ROOM_DESC_MAX_LENGTH + 1);
    room->owner = owner;
    room->lock = createReadWriteLock();
    room->clients[0] = owner;
    for (int i = 1; i < MAX_USERS_PER_ROOM; i++) {
        room->clients[i] = NULL;
    }
    return room;
}

void destroyRoom(Room *room) {
    destroyReadWriteLock(room->lock);
    free(room);
}

/**
 * \brief Tries to insert the given room in the rooms array.
 *
 * It fails if a room with the same name as the given room already exists (error code: 1).<br>
 * It fails if the maximum amount of rooms is already reached (error code: 2).
 *
 * \param roomToInsert The new room to add to the rooms array
 * \return 0 if insertion is a success, else the error code
 */
int tryInsertRoom(Room *roomToInsert) {
    Room *room = findRoomByName(roomToInsert->name);
    /* Name already taken */
    if (room != NULL) {
        return 1;
    }

    int roomId = findFirstFreeRoomSlot();
    /* No room slot available */
    if (roomId == -1) {
        return 2;
    }

    rooms[roomId] = roomToInsert;
    return 0;
}

void handleRoomCreationRequest(Client *client, struct PacketCreateRoom *packet) {
    SYNC_CLIENT_READ(int isInRoom = client->room != NULL);
    if (isInRoom) {
        Packet errorPacket = NewPacketServerErrorMessage;
        memcpy(errorPacket.asServerErrorMessagePacket.message, "You're already in a room. First leave the room.", 48);
        sendPacket(client->socket, &errorPacket);
    } else if (strlen(packet->roomName) == 0) {
        Packet errorPacket = NewPacketServerErrorMessage;
        memcpy(errorPacket.asServerErrorMessagePacket.message, "The room name can't be empty.", 30);
        sendPacket(client->socket, &errorPacket);
    } else {
        Room *room = createRoom(client, packet->roomName, packet->roomDesc);
        SYNC_ROOMS_WRITE(int error = tryInsertRoom(room));
        if (error == 1) {
            destroyRoom(room);
            Packet errorPacket = NewPacketServerErrorMessage;
            memcpy(errorPacket.asServerErrorMessagePacket.message, "This room name is already used.", 32);
            sendPacket(client->socket, &errorPacket);
        } else if (error == 2) {
            destroyRoom(room);
            Packet errorPacket = NewPacketServerErrorMessage;
            memcpy(errorPacket.asServerErrorMessagePacket.message, "The maximum amount of rooms is reached.", 40);
            sendPacket(client->socket, &errorPacket);
        } else {
            client->room = room; // SAFE, because we are room owner
            Packet joinPacket = NewPacketJoin;
            getClientUsername(client, joinPacket.asJoinPacket.username);
            sendPacket(client->socket, &joinPacket);
        }
    }
}

void handleRoomJoinRequest(Client *client, struct PacketJoinRoom *packet) {
    acquireRead(clientsLock);
    if (client->room != NULL) {
        releaseRead(clientsLock);

        Packet errorPacket = NewPacketServerErrorMessage;
        memcpy(errorPacket.asServerErrorMessagePacket.message, "You're already in a room. First leave the room.", 48);
        sendPacket(client->socket, &errorPacket);
        return;
    }
    releaseRead(clientsLock);

    acquireRead(roomsLock);
    Room *room = findRoomByName(packet->roomName);
    if (room == NULL) {
        releaseRead(roomsLock);

        Packet errorPacket = NewPacketServerErrorMessage;
        memcpy(errorPacket.asServerErrorMessagePacket.message, "This room does not exist.", 26);
        sendPacket(client->socket, &errorPacket);
        return;
    }

    acquireWrite(room->lock);
    releaseRead(roomsLock);

    int slot = findFirstFreeSlotForRoom(room);
    if (slot == -1) {
        releaseWrite(room->lock);

        Packet errorPacket = NewPacketServerErrorMessage;
        memcpy(errorPacket.asServerErrorMessagePacket.message, "This room is full.", 19);
        sendPacket(client->socket, &errorPacket);
        return;
    }

    client->room = room;
    room->clients[slot] = client;
    releaseWrite(room->lock);

    Packet joinPacket = NewPacketJoin;
    getClientUsername(client, joinPacket.asJoinPacket.username);

    syncBroadcastRoom(&joinPacket, room);
}

void handleRoomLeaveRequest(Client *client) {
    acquireWrite(clientsLock);
    Room *room = client->room;

    if (room == NULL) {
        releaseRead(clientsLock);
        Packet errorPacket = NewPacketServerErrorMessage;
        memcpy(errorPacket.asServerErrorMessagePacket.message, "You're not in a room.", 48);
        sendPacket(client->socket, &errorPacket);
        return;
    }

    acquireWrite(room->lock);

    if (client == room->owner) {
        Packet leavePacket = NewPacketLeave;
        for (int i = 0; i < MAX_USERS_PER_ROOM; i++) {
            if (room->clients[i] != NULL) {
                memcpy(leavePacket.asLeavePacket.username, room->clients[i]->username, USERNAME_MAX_LENGTH + 1);
                broadcastRoom(&leavePacket, room);
                room->clients[i]->room = NULL;
            }
        }
        releaseWrite(clientsLock);

        SYNC_ROOMS_WRITE(
            int roomId = findRoomId(room);
            rooms[roomId] = NULL;
        );

        releaseWrite(room->lock);
        destroyReadWriteLock(room->lock);
        free(rooms[roomId]);
    } else {
        client->room = NULL;
        releaseWrite(clientsLock);

        Packet leavePacket = NewPacketLeave;
        memcpy(leavePacket.asLeavePacket.username, client->username, USERNAME_MAX_LENGTH + 1);
        broadcastRoom(&leavePacket, room);

        int slot = 0;
        while (slot < MAX_USERS_PER_ROOM && room->clients[slot] != client) {
            slot++;
        }

        if (slot < MAX_USERS_PER_ROOM) { // Should always be true
            room->clients[slot] = NULL;
        }

        releaseWrite(room->lock);
    }
}