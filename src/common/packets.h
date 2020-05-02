#ifndef C_CHAT_PACKETS_H
#define C_CHAT_PACKETS_H

#include "constants.h"
#include "sockets.h"

struct PacketJoin {
    char type;
    char username[USERNAME_MAX_LENGTH + 1];
}; // NewJoinPacket = { JOIN_MESSAGE_TYPE };

struct PacketLeave {
    char type;
    char username[USERNAME_MAX_LENGTH + 1];
}; // NewLeavePacket = { LEAVE_MESSAGE_TYPE };

struct PacketText {
    char type;
    char message[MSG_MAX_LENGTH + 1];
    char username[USERNAME_MAX_LENGTH + 1];
}; // NewPacketText = { TEXT_MESSAGE_TYPE };

struct PacketDefineUsername {
    char type;
    char username[USERNAME_MAX_LENGTH + 1];
}; // NextPacketDefineUsername = { DEFINE_USERNAME_MESSAGE_TYPE };

struct PacketServerErrorMessage {
    char type;
    char message[MSG_MAX_LENGTH + 1];
}; // NewPacketServerErrorMessage = { SERVER_ERROR_MESSAGE_TYPE };

struct PacketUsernameChanged {
    char type;
    char oldUsername[USERNAME_MAX_LENGTH + 1];
    char newUsername[USERNAME_MAX_LENGTH + 1];
}; // NewPacketUsernameChanged = { USERNAME_CHANGED_MESSAGE_TYPE };

typedef union Packet {
    char type;
    char* raw;
    struct PacketJoin joinPacket;
    struct PacketLeave leavePacket;
    struct PacketText textPacket;
    struct PacketDefineUsername defineUsernamePacket;
    struct PacketServerErrorMessage serverErrorMessagePacket;
    struct PacketUsernameChanged usernameChangedPacket;
} Packet;

int receiveNextPacket(SocketInfo socket, Packet* packet);
int sendPacket(SocketInfo socket, Packet* packet);

#endif //C_CHAT_PACKETS_H
