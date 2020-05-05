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
/** This instance is ued to create a new PacketQuit */
extern const union Packet NewPacketQuit;

/**
 * \class PacketFileUploadRequest
 * \brief This packet is sent to server to ask for file upload
 */
struct PacketFileUploadRequest {
    char type;
    long long fileSize;
};
/** This instance is used to create a new PacketFileUploadRequest */
extern const union Packet NewPacketFileUploadRequest;

/**
 * \class PacketFileDownloadRequest
 * \brief This packet is sent to server to ask for file download
 */
struct PacketFileDownloadRequest {
    char type;
    int fileId;
};
/** This instance is used to create new PacketFileDownloadRequest */
extern const union Packet NewPacketFileDownloadRequest;

/**
 * \class PacketFileTransferValidation
 * \brief This packet is sent to client by server to accept/reject a file transaction
 */
struct PacketFileTransferValidation {
    char type;
    char accepted;
    int id;
};
/** This instance is used to create new PacketFileTransferValidation */
extern const union Packet NewPacketFileTransferValidation;

/**
 * \class PacketFileDataTransfer
 * \brief This packet is exchanged between client and server to transfer file content
 */
struct PacketFileDataTransfer {
    char type;
    int id;
    char data[FILE_TRANSFER_CHUNK_SIZE];
};
/** This instance is used to create a new PacketFileDataTransfert */
extern const union Packet NewPacketFileDataTransfer;

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
    struct PacketFileUploadRequest asFileUploadRequestPacket;
    struct PacketFileDownloadRequest asFileDownloadRequestPacket;
    struct PacketFileTransferValidation asFileTransferValidationPacket;
    struct PacketFileDataTransfer asFileDataTransferPacket;
} Packet;

/**
 * \brief Receives the next packet incoming on the given socket
 *
 * \param socket The socket to receive the packet on
 * \param packet The packet to fill in with received data
 *
 * \return the number of bytes received (can be lower than or equal to 0)
 */
int receiveNextPacket(Socket socket, Packet* packet);

/**
 * \brief Sends the given packet on the given socket
 *
 * \param socket The socket to send the packet on
 * \param packet The packet to send on the socket
 *
 * \return the number of bytes sent (can le lower than or equal to 0)
 */
int sendPacket(Socket socket, Packet* packet);

#endif //C_CHAT_PACKETS_H