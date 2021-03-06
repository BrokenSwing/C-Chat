/**
 * \file constants.h
 * \brief Defines constants used on client and server (used for consistency)
 */

#ifndef C_CHAT_CONSTANTS_H
#define C_CHAT_CONSTANTS_H

//---------------------------------------------------------//
//                     SIZES DEFINITION                    //
//---------------------------------------------------------//

/**
 * \def MSG_MAX_LENGTH
 * \brief Maximum message size.
 */
#define MSG_MAX_LENGTH 250

/**
 * \def USERNAME_MAX_LENGTH
 * \brief Maximum length for a client username
 */
#define USERNAME_MAX_LENGTH 20

/**
 * \def FILE_TRANSFER_CHUNK_SIZE
 * \brief Size of data chunks for file transfer
 */
#define FILE_TRANSFER_CHUNK_SIZE 200

/**
 * \def MAX_FILE_SIZE_UPLOAD
 * \brief Maximum allowed size for file upload (bytes)
 */
// 15 Ko
#define MAX_FILE_SIZE_UPLOAD 800000000

/**
 * \def MAX_CONCURRENT_FILE_TRANSFER
 * \brief Maximum concurrent transfer of file allowed per client
 */
#define MAX_CONCURRENT_FILE_TRANSFER 2

/**
 * \def ROOM_NAME_MAX_LENGTH
 * \brief The maximum length for a name of a room
 */
#define ROOM_NAME_MAX_LENGTH 20

/**
 * \def ROOM_DESC_MAX_LENGTH
 * \brief The maximum length for the description of a room
 */
#define ROOM_DESC_MAX_LENGTH 120

/**
 * \def MAX_USERS_PER_ROOM
 * \brief The maximum amount of users connected simultaneously to a room
 */
#define MAX_USERS_PER_ROOM 10

/**
 * \def NUMBER_ROOM_MAX
 * \brief The maximum of supported rooms
 */
#define NUMBER_ROOM_MAX 20

//---------------------------------------------------------//
//              MESSAGES TYPES DEFINITION                  //
//---------------------------------------------------------//

/**
 * \def JOIN_MESSAGE_TYPE
 * \brief An integer representing a join message
 */
#define JOIN_MESSAGE_TYPE 0

/**
 * \def LEAVE_MESSAGE_TYPE
 * \brief An integer representing a leave message
 */
#define LEAVE_MESSAGE_TYPE 1

/**
 * \def TEXT_MESSAGE_TYPE
 * \brief An integer representing a text message
 */
#define TEXT_MESSAGE_TYPE 2

/**
 * \def DEFINE_USERNAME_MESSAGE_TYPE
 * \brief An integer representing a message meant to define the client username
 */
#define DEFINE_USERNAME_MESSAGE_TYPE 3

/**
 * \def SERVER_ERROR_MESSAGE_TYPE
 * \brief An integer representing an error message sent by server
 */
#define SERVER_ERROR_MESSAGE_TYPE 4

/**
 * \def USERNAME_CHANGED_MESSAGE_TYPE
 * \brief An integer representing a message meant to notify clients a client changed its username
 */
#define USERNAME_CHANGED_MESSAGE_TYPE 5

/**
 * \def QUIT_MESSAGE_TYPE
 * \brief An integer representing a message meant to notify the server the client wants to quit the room
 */
#define QUIT_MESSAGE_TYPE 6

/**
 * \def FILE_UPLOAD_REQUEST_MESSAGE_TYPE
 * \brief An integer representing a message meant to ask the server for file upload
 */
#define FILE_UPLOAD_REQUEST_MESSAGE_TYPE 7

/**
 * \def FILE_DOWNLOAD_REQUEST_MESSAGE_TYPE
 * \brief An integer representing a message meant to ask the server for file download
 */
#define FILE_DOWNLOAD_REQUEST_MESSAGE_TYPE 8

/**
 * \def FILE_UPLOAD_VALIDATION_MESSAGE_TYPE
 * \brief An integer representing a message sent by server to client to validate/invalidate its file upload request
 */
#define FILE_UPLOAD_VALIDATION_MESSAGE_TYPE 9

/**
 * \def FILE_DATA_TRANSFER_MESSAGE_TYPE
 * \brief An integer representing a message that transfers file data between client and server
 */
#define FILE_DATA_TRANSFER_MESSAGE_TYPE 10

/**
 * \def FILE_DOWNLOAD_VALIDATION_MESSAGE_TYPE
 * \brief An integer representing a message sent by server to client to validate/invalidate its file download request
 */
#define FILE_DOWNLOAD_VALIDATION_MESSAGE_TYPE 11

/**
 * \def FILE_TRANSFER_CANCEL_MESSAGE_TYPE
 * \brief An integer representing a message sent between client/server to cancel a file transfer
 */
#define FILE_TRANSFER_CANCEL_MESSAGE_TYPE 12

/**
 * \def SERVER_SUCCESS_MESSAGE_TYPE
 * \brief An integer representing a success message sent by server
 */
#define SERVER_SUCCESS_MESSAGE_TYPE 13

/**
 * \def CREATE_ROOM_MESSAGE_TYPE
 * \brief An integer representing a message sent by client to ask for room creation
 */
#define CREATE_ROOM_MESSAGE_TYPE 14

/**
 * \def JOIN_ROOM_MESSAGE_TYPE
 * \brief An integer representing a message sent by client to join room
 */
#define JOIN_ROOM_MESSAGE_TYPE 15

/**
 * \def LEAVE_ROOM_MESSAGE_TYPE
 * \brief An integer representing a message sent by client to leave a room
 */
#define LEAVE_ROOM_MESSAGE_TYPE 16

/**
 * \def LIST_ROOMS_MESSAGE_TYPE
 * \brief An integer representing a message sent by client to ask existing rooms list
 */
#define LIST_ROOMS_MESSAGE_TYPE 17

#endif //C_CHAT_CONSTANTS_H
