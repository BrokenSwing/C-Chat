/**
 * \file communication.h
 * \brief Regroup functions that aim to communicate with clients
 */

#ifndef C_CHAT_COMMUNICATION_H
#define C_CHAT_COMMUNICATION_H

#include "../common/packets.h"
#include "server.h"

/**
 * \brief Broadcasts a packet to all clients
 *
 * \param packet The packet to send to all client
 */
void broadcast(Packet* packet);

/**
 * \brief Processes a received PacketText
 *
 * \param client The client who sent the packet
 * @param packet The received packet
 */
void handleTextMessageRelay(Client* client, struct PacketText* packet);

#endif //C_CHAT_COMMUNICATION_H
