#ifndef C_CHAT_PACKETS_H
#define C_CHAT_PACKETS_H

#include "constants.h"
#include "sockets.h"

struct PacketJoin {
    char type;
    char username[USERNAME_MAX_LENGTH + 1];
};
extern const union Packet NewJoinPacket;

struct PacketLeave {
    char type;
    char username[USERNAME_MAX_LENGTH + 1];
};
extern const union Packet NewLeavePacket;

struct PacketText {
    char type;
    char message[MSG_MAX_LENGTH + 1];
    char username[USERNAME_MAX_LENGTH + 1];
};
extern const union Packet NewPacketText;

struct PacketDefineUsername {
    char type;
    char username[USERNAME_MAX_LENGTH + 1];
};
extern const union Packet NewPacketDefineUsername;

struct PacketServerErrorMessage {
    char type;
    char message[MSG_MAX_LENGTH + 1];
};
extern const union Packet NewPacketServerErrorMessage;

struct PacketUsernameChanged {
    char type;
    char oldUsername[USERNAME_MAX_LENGTH + 1];
    char newUsername[USERNAME_MAX_LENGTH + 1];
};
extern const union Packet NewPacketUsernameChanged;

typedef union Packet {
    char type;
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