/**
 * \file client.h
 * \brief Header file for client.c
 */

#ifndef C_CHAT_CLIENT_H
#define C_CHAT_CLIENT_H

#include "../common/threads.h"

extern Socket clientSocket;
extern char* uploadFilename;
extern Thread uploadThread;
extern char* downloadBuffer;
extern long long downloadFileSize;
extern long long downloadedSize;
extern unsigned int downloadFileId;

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

/**
 * \brief Handles the command represented by the given string
 *
 * \param command A command typed by the user
 */
void commandHandler(const char* command);

/**
 * \brief Notifies the server of username change
 *
 * The server can reject the new username.
 *
 * \param username The new username to use
 */
void setUsername(const char* username);

#endif //C_CHAT_CLIENT_H
