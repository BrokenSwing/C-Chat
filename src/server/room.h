#ifndef C_CHAT_ROOM_H
#define C_CHAT_ROOM_H

#include "server.h"

/**
 * \brief Processes a received PacketCreateRoom.
 *
 * \param client The client who sent the packet
 * \param packet The received packet
 */
void handleRoomCreationRequest(Client* client, struct PacketCreateRoom* packet);

/**
 * \brief Processes a received PacketJoinRoom
 *
 * \param client The client who sent the packet
 * \param packet The received packet
 */
void handleRoomJoinRequest(Client* client, struct PacketJoinRoom* packet);

/**
 * \brief Processes a received PacketLeaveRoom
 *
 * \param client The client who sent the packet
 * \param packet The received packet
 */
void handleRoomLeaveRequest(Client* client, struct PacketLeaveRoom* packet);

#endif //C_CHAT_ROOM_H
