/**
 * \file packets.h
 * \brief Defines types of packets that are exchanged between client and server
 */

#ifndef C_CHAT_PACKETS_H
#define C_CHAT_PACKETS_H

#include "constants.h"
#include "sockets.h"

/**
 * \class PacketJoin
 * \brief This packet is sent to all client of a room when a client joins the room
 */
struct PacketJoin {
    char type;
    char username[USERNAME_MAX_LENGTH + 1];
};
/** This instance is used to create a new PacketJoin */
extern const union Packet NewPacketJoin;

/**
 * \class PacketLeave
 * \brief This packet is sent to all client of a room when a client leaves the room
 */
struct PacketLeave {
    char type;
    char username[USERNAME_MAX_LENGTH + 1];
};
/** This instance is used to create a new PacketLeave */
extern const union Packet NewPacketLeave;

/**
 * \class PacketText
 * \brief This packet is sent to all clients of a room when a client sends a message
 *
 * This message is also used to send a text message from client to server, but in this case
 * the username part is ignored by the server.
 */
struct PacketText {
    char type;
    char message[MSG_MAX_LENGTH + 1];
    char username[USERNAME_MAX_LENGTH + 1];
};
/** This instance is used to create a new PacketText */
extern const union Packet NewPacketText;

/**
 * \class PacketDefineUsername
 * \brief This packet is sent to server by client to re-define its username
 */
struct PacketDefineUsername {
    char type;
    char username[USERNAME_MAX_LENGTH + 1];
};
/** This instance is used create a new PacketDefineUsername */
extern const union Packet NewPacketDefineUsername;

/**
 * \class PacketServerErrorMessage
 * \brief This packet is sent by server to an user when an error occurs
 */
struct PacketServerErrorMessage {
    char type;
    char message[MSG_MAX_LENGTH + 1];
};
/** This instance is used to create a new PacketServerErrorMessage */
extern const union Packet NewPacketServerErrorMessage;

/**
 * \class PacketUsernameChanged
 * \brief This packet is sent to all client of a room when an user of the room changes its username
 */
struct PacketUsernameChanged {
    char type;
    char oldUsername[USERNAME_MAX_LENGTH + 1];
    char newUsername[USERNAME_MAX_LENGTH + 1];
};
/** This instance is used to create a new PacketUsernameChanged */
extern const union Packet NewPacketUsernameChanged;

/**
 * \class PacketQuit
 * \brief This packet is sent to server by client to notify him he wants to quit the room
 */
struct PacketQuit {
    char type;
};
extern const union Packet NewPacketQuit;

/**
 * \class Packet
 * \brief A generic union type for packets
 */
typedef union Packet {
    char type;
    struct PacketJoin asJoinPacket;
    struct PacketLeave asLeavePacket;
    struct PacketText asTextPacket;
    struct PacketDefineUsername asDefineUsernamePacket;
    struct PacketServerErrorMessage asServerErrorMessagePacket;
    struct PacketUsernameChanged asUsernameChangedPacket;
    struct PacketQuit asQuitPacket;
} Packet;

/**
 * \brief Receives the next packet incoming on the given socket
 *
 * \param socket The socket to receive the packet on
 * \param packet The packet to fill in with received data
 *
 * \return the number of bytes received (can be lower than or equal to 0)
 */
int receiveNextPacket(SocketInfo socket, Packet* packet);

/**
 * \brief Sends the given packet on the given socket
 *
 * \param socket The socket to send the packet on
 * \param packet The packet to send on the socket
 *
 * \return the number of bytes sent (can le lower than or equal to 0)
 */
int sendPacket(SocketInfo socket, Packet* packet);

#endif //C_CHAT_PACKETS_H