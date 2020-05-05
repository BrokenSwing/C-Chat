#ifndef C_CHAT_CLIENT_INFO_H
#define C_CHAT_CLIENT_INFO_H

#include "server.h"

/**
 * Validates the format of the given username.
 *
 * \param username The username to validate
 *
 * \return 0 if username if invalid, else 1
 */
int validateUsername(const char* username);

/**
 * \brief Defines the username of the client with the given id to the given username.
 *
 * Expects the username to have a valid username format.
 *
 * \param client The client to set the new username to
 * \param newUsername The new username of the client
 */
void setClientUsername(Client* client, const char* newUsername);

/**
 * \brief Reads the username of the given client.
 *
 * \param client The client to get username of
 * \param dest The buffer to copy client username in
 */
void getClientUsername(Client* client, char* dest);

/**
 * \brief Processes a received PacketDefineUsername
 *
 * \param client The client who sent the packet
 * \param packet The received packet
 */
void handleUsernameChange(Client* client, struct PacketDefineUsername* packet);

#endif //C_CHAT_CLIENT_INFO_H
