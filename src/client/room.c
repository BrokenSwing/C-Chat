#include "room.h"
#include "../common/constants.h"
#include <string.h>
#include "ui.h"
#include "../common/packets.h"
#include "client.h"

void createRoom(const char* command) {
    char roomName[ROOM_NAME_MAX_LENGTH + 1];
    char roomDesc[ROOM_DESC_MAX_LENGTH + 1];

    int i = 0;
    while (strlen(command) > 0 && command[0] != ' ' && i <= ROOM_NAME_MAX_LENGTH) {
        roomName[i] = command[0];
        command += 1;
        i += 1;
    }
    roomName[i] = '\0';

    // We didn't stop because we reached a space, it means we didn't consume all the provided channel name
    if (strlen(command) > 0 && command[0] != ' ') {
        ui_errorMessage("Room name is too long. You should not exceed 20 characters.");
        return;
    }

    /* Skip spaces */
    while (strlen(command) > 0 && command[0] == ' ') {
        command += 1;
    }

    i = 0;
    while (strlen(command) > 0 && i <= ROOM_DESC_MAX_LENGTH) {
        roomDesc[i] = command[0];
        command += 1;
        i += 1;
    }
    roomDesc[i] = '\0';

    // We didn't stop because we reached the end of line, it means we didn't consume all the provided channel description
    if (strlen(command) > 0) {
        ui_errorMessage("Room description is too long. You should not exceed 120 characters.");
        return;
    }

    /* We suppress trailing spaces */
    while (roomDesc[strlen(roomDesc) - 1] == ' ') {
        roomDesc[strlen(roomDesc) - 1] = '\0';
    }

    Packet packet = NewPacketCreateRoom;
    memcpy(packet.asCreateRoomPacket.roomName, roomName, ROOM_NAME_MAX_LENGTH + 1);
    memcpy(packet.asCreateRoomPacket.roomDesc, roomDesc, ROOM_DESC_MAX_LENGTH + 1);
    sendPacket(clientSocket, &packet);
}

void joinRoom(const char* command) {
    char roomName[ROOM_NAME_MAX_LENGTH + 1];

    int i = 0;
    while (strlen(command) > 0 && command[0] != ' ' && i <= ROOM_NAME_MAX_LENGTH) {
        roomName[i] = command[0];
        command += 1;
        i += 1;
    }
    roomName[i] = '\0';

    Packet packet = NewPacketJoinRoom;
    memcpy(packet.asJoinRoomPacket.roomName, roomName, ROOM_NAME_MAX_LENGTH + 1);
    sendPacket(clientSocket, &packet);
}

void leaveRoom() {
    Packet packet = NewPacketLeaveRoom;
    sendPacket(clientSocket, &packet);
}

void listRooms() {
    Packet packet = NewPacketListRooms;
    sendPacket(clientSocket, &packet);
}