#ifndef C_CHAT_FILE_TRANSFER_H
#define C_CHAT_FILE_TRANSFER_H

#include "server.h"
#include "../common/packets.h"

/**
 * \brief Processes a received PacketFileUploadRequest
 *
 * \param client The client who sent the packet
 * \param packet The received packet
 */
void handleUploadRequest(Client* client, struct PacketFileUploadRequest* packet);

/**
 * \brief Generates an unique file id for a new file upload
 *
 * \return the generated unique file id
 */
unsigned int generateNewFileId();

/**
 * \brief Processes a received PacketFileDataTransfer
 *
 * \param client The client who sent the packet
 * \param packet The received packet
 */
void handleFileDataUpload(Client* client, struct PacketFileDataTransfer* packet);

/**
 * \brief Processes a received PacketFileDownloadRequest
 *
 * \param client The client who sent the packet
 * \param packet The received packet
 */
void handleDownloadRequest(Client* client, struct PacketFileDownloadRequest* packet);

#endif //C_CHAT_FILE_TRANSFER_H
