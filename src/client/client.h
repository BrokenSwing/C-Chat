#include "../common/threads.h"

#ifndef C_CHAT_CLIENT_H
#define C_CHAT_CLIENT_H

/**
 * \brief An entry point for a thread that gets user input and send it to server
 *
 * Gets user input and sent it to the server in an appropriate way.
 *
 * \param data a NULL pointer
 * \return never
 */
THREAD_ENTRY_POINT sendMessage(void* data);

/**
 * \brief Receives messages from the server and display them to the client
 */
void receiveMessages();

#endif //C_CHAT_CLIENT_H
