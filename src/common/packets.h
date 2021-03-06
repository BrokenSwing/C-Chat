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
    unsigned int fileId;
};
/** This instance is used to create new PacketFileDownloadRequest */
extern const union Packet NewPacketFileDownloadRequest;

/**
 * \class PacketFileUploadValidation
 * \brief This packet is sent to client by server to accept/reject a file upload
 */
struct PacketFileUploadValidation {
    char type;
    char accepted;
    unsigned int id;
};
/** This instance is used to create new PacketFileUploadValidation */
extern const union Packet NewPacketFileUploadValidation;

/**
 * \class PacketFileDownloadValidation
 * \brief This packet is sent to client by server to accept/reject a file download
 */
struct PacketFileDownloadValidation {
    char type;
    char accepted;
    unsigned int fileId;
    long long fileSize;
};
/** This instance is used to create new PacketFileDownloadValidation */
extern const union Packet NewPacketFileDownloadValidation;

/**
 * \class PacketFileDataTransfer
 * \brief This packet is exchanged between client and server to transfer file content
 */
struct PacketFileDataTransfer {
    char type;
    unsigned int id;
    char data[FILE_TRANSFER_CHUNK_SIZE];
};
/** This instance is used to create a new PacketFileDataTransfer */
extern const union Packet NewPacketFileDataTransfer;

/**
 * \class PacketFileTransferCancel
 * \brief This packet is sent between client/server to cancel a file transfer
 */
struct PacketFileTransferCancel {
    char type;
    unsigned int id;
};
/** This instance is used to create a new PacketFileTransferCancel */
extern const union Packet NewPacketFileTransferCancel;

/**
 * \class PacketServerSuccess
 * \brief This packet is sent by server to client on success
 */
struct PacketServerSuccess {
    char type;
    char message[MSG_MAX_LENGTH +1];
};
/** This instance is used to create a new PacketServerSuccess */
extern const union Packet NewPacketServerSuccess;

/**
 * \class PacketCreateRoom
 * \brief This packet is sent by client to create a room
 */
struct PacketCreateRoom {
    char type;
    char roomName[ROOM_NAME_MAX_LENGTH + 1];
    char roomDesc[ROOM_DESC_MAX_LENGTH + 1];
};
/** This instance is used to create a new PacketCreateRoom  */
extern const union Packet NewPacketCreateRoom;

/**
 * \class PacketJoinRoom
 * \brief This packet is sent by client to join a room
 */
struct PacketJoinRoom {
    char type;
    char roomName[ROOM_NAME_MAX_LENGTH + 1];
};
/** This instance is used to create a new PacketJoinRoom */
extern const union Packet NewPacketJoinRoom;

/**
 * \class PacketLeaveRoom
 * \brief This packet is sent by client to leave its room
 */
struct PacketLeaveRoom {
    char type;
};
/** This instance is used to create a new PacketLeaveRoom */
extern const union Packet NewPacketLeaveRoom;

/**
 * \class PacketListRooms
 * \brief This packet is sent by client to request existing rooms list
 */
struct PacketListRooms {
    char type;
};
/** This instance is used to create a new PacketListRooms */
extern const union Packet NewPacketListRooms;

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
    struct PacketFileUploadRequest asFileUploadRequestPacket;
    struct PacketFileDownloadRequest asFileDownloadRequestPacket;
    struct PacketFileUploadValidation asFileUploadValidationPacket;
    struct PacketFileDownloadValidation asFileDownloadValidationPacket;
    struct PacketFileDataTransfer asFileDataTransferPacket;
    struct PacketFileTransferCancel asFileTransferCancelPacket;
    struct PacketServerSuccess asServerSuccessMessagePacket;
    struct PacketCreateRoom asCreateRoomPacket;
    struct PacketJoinRoom asJoinRoomPacket;
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