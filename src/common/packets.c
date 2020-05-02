#include "packets.h"

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
        default:
            return 0;
    }
}

int receiveNextPacket(SocketInfo socket, Packet* packet) {
    int bytesReceived;
    bytesReceived = receiveFrom(socket, &(packet->type), sizeof(Packet));
    if (bytesReceived <= 0) {
        return bytesReceived;
    }

    return bytesReceived;
}

int sendPacket(SocketInfo socket, Packet* packet) {
    unsigned int realSize = packets_sizeOf(packet);
    return sendTo(socket, packet->raw, realSize);
}