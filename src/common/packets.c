#include "packets.h"

const union Packet NewPacketJoin = { JOIN_MESSAGE_TYPE };
const union Packet NewPacketLeave = { LEAVE_MESSAGE_TYPE };
const union Packet NewPacketText = { TEXT_MESSAGE_TYPE };
const union Packet NewPacketDefineUsername = { DEFINE_USERNAME_MESSAGE_TYPE };
const union Packet NewPacketServerErrorMessage = { SERVER_ERROR_MESSAGE_TYPE };
const union Packet NewPacketUsernameChanged = { USERNAME_CHANGED_MESSAGE_TYPE };
const union Packet NewPacketQuit = { QUIT_MESSAGE_TYPE };
const union Packet NewPacketFileUploadRequest = { FILE_UPLOAD_REQUEST_MESSAGE_TYPE };
const union Packet NewPacketFileDownloadRequest = { FILE_DOWNLOAD_REQUEST_MESSAGE_TYPE };
const union Packet NewPacketFileTransferValidation = { FILE_TRANSFER_VALIDATION_MESSAGE_TYPE };
const union Packet NewPacketFileDataTransfer = { FILE_DATA_TRANSFER_MESSAGE_TYPE };

/**
 * \brief Retrieves the real size of the underlying type
 *
 * \param packet The packet to get real size of
 * \return the real size of the underlying type or 0 if the type isn't known
 */
unsigned int packets_sizeOf(Packet* packet) {
    switch (packet->type) {
        case JOIN_MESSAGE_TYPE:
            return sizeof(struct PacketJoin);
        case LEAVE_MESSAGE_TYPE:
            return sizeof(struct PacketLeave);
        case TEXT_MESSAGE_TYPE:
            return sizeof(struct PacketText);
        case DEFINE_USERNAME_MESSAGE_TYPE:
            return sizeof(struct PacketDefineUsername);
        case SERVER_ERROR_MESSAGE_TYPE:
            return sizeof(struct PacketServerErrorMessage);
        case USERNAME_CHANGED_MESSAGE_TYPE:
            return sizeof(struct PacketUsernameChanged);
        case QUIT_MESSAGE_TYPE:
            return sizeof(struct PacketQuit);
        case FILE_UPLOAD_REQUEST_MESSAGE_TYPE:
            return sizeof(struct PacketFileUploadRequest);
        case FILE_DOWNLOAD_REQUEST_MESSAGE_TYPE:
            return sizeof(struct PacketFileDownloadRequest);
        case FILE_TRANSFER_VALIDATION_MESSAGE_TYPE:
            return sizeof(struct PacketFileTransferValidation);
        case FILE_DATA_TRANSFER_MESSAGE_TYPE:
            return sizeof(struct PacketFileDataTransfer);
        default:
            return 0;
    }
}

int receiveNextPacket(Socket socket, Packet* packet) {
    int bytesReceived;
    bytesReceived = receiveFrom(socket, &(packet->type), sizeof(char));
    if (bytesReceived <= 0) {
        return bytesReceived;
    }

    unsigned packetSize = packets_sizeOf(packet) - sizeof(char);
    if (packetSize < 0) { // Unknown packet type
        return bytesReceived;
    }

    if (packetSize == 0) { // No more content to read for this packet
        return bytesReceived;
    }

    bytesReceived = receiveFrom(socket, &(packet->type) + 1, packetSize);
    if (bytesReceived <= 0) {
        return bytesReceived;
    }

    return bytesReceived;
}

int sendPacket(Socket socket, Packet* packet) {
    unsigned int realSize = packets_sizeOf(packet);
    return sendTo(socket, (char*) packet, realSize);
}