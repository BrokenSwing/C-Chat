/**
 * \file client.h
 * \brief Header file for client.c
 */

#ifndef C_CHAT_CLIENT_H
#define C_CHAT_CLIENT_H

#include "../common/threads.h"
#include "../common/sockets.h"

struct UploadData {
    char* uploadFilename;
    Thread uploadThread;
};

struct DownloadData {
    char* downloadBuffer;
    long long downloadFileSize;
    long long downloadedSize;
    unsigned int downloadFileId;
};

extern Socket clientSocket;
/* Upload */
extern struct UploadData uploadData[MAX_CONCURRENT_FILE_TRANSFER]; // TODO: Sync access
/* Download */
extern struct DownloadData downloadData[MAX_CONCURRENT_FILE_TRANSFER]; // TODO: Sync access


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
