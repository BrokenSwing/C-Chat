#ifndef C_CHAT_FILE_TRANSFER_H
#define C_CHAT_FILE_TRANSFER_H

#include "../common/packets.h"

/**
 * \brief Tries to send a file upload request to the server.
 *
 * If the specified file doesn't exist or can't be sent, this function displays
 * an error message to the client.
 *
 * \param filename The name of the file to send
 */
void sendFileUploadRequest(const char* filename);

/**
 * \brief Sends a file download request to the server.
 *
 * \param fileId The id of the file request download of
 */
void sendFileDownloadRequest(unsigned int fileId);

/**
 * \brief Processes the received PacketFileTransferCancel.
 *
 * \param packet The received packet
 */
void handleFileDownloadCancel(struct PacketFileTransferCancel* packet);

/**
 * \brief Processes the received PacketFileUploadValidation.
 *
 * \param packet The received packet
 */
void handleFileUploadValidation(struct PacketFileUploadValidation* packet);

/**
 * \brief Processes received PacketFileDataTransfer;
 *
 * \param packet The received packet
 */
void handleFileData(struct PacketFileDataTransfer* packet);

/**
 * \brief Processed received PacketFileDownloadValidation.
 *
 * \param packet The received packet
 */
void handleFileDownloadValidation(struct PacketFileDownloadValidation* packet);

#endif //C_CHAT_FILE_TRANSFER_H
