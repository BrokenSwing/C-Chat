#ifndef C_CHAT_HANDSHAKE_H
#define C_CHAT_HANDSHAKE_H

#include "server.h"

/**
 * \brief Scan clients slots for an available slot.
 *
 * Iterates over clients slots to find an empty slot.
 *
 * \return -1 if no empty slot was found, else an available slot id
 */
int scanForFreeSocketSlot();

/**
 * \brief Initialize a client connection to be ready to discuss.
 *
 * Receives the client username and performs checks to ensure its validity.
 *
 * \param client A pointer to an integer which contains the slot id of the client to initialize
 * \return EXIT_SUCCESS if client initialized correctly, else EXIT_FAILURE
 */
int initClientConnection(Client* client);

/**
 * \brief Disconnects the client and free allocated memory
 *
 * \param id The client slot id
 */
void disconnectClient(int id);

#endif //C_CHAT_HANDSHAKE_H
