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
 * \def MAX_CONCURRENT_FILES_EXCHANGE_PER_CLIENT
 * \brief Maximum concurrent files exchange a client can operate with the server
 *
 * A file exchange is either receiving or sending a file.
 */
#define MAX_CONCURRENT_FILES_EXCHANGE_PER_CLIENT 1

/**
 * \def MESSAGE_TYPE_OVERHEAD
 * \brief The number of bytes the type of the message is
 */
#define MESSAGE_TYPE_OVERHEAD 1

//---------------------------------------------------------//
//              MESSAGES TYPES DEFINITION                  //
//---------------------------------------------------------//

/**
 * \def JOIN_MESSAGE_TYPE
 * \brief An integer representing a join message
 *
 * Content of packets of this type must contain :
 *  - the username of the joining user : (USERNAME_MAX_LENGTH + 1) bytes long
 */
#define JOIN_MESSAGE_TYPE 0

/**
 * \def LEAVE_MESSAGE_TYPE
 * \brief An integer representing a leave message
 *
 * Content of packets of this type must contain :
 *  - the username of the joining user : (USERNAME_MAX_LENGTH + 1) bytes long
 */
#define LEAVE_MESSAGE_TYPE 1

/**
 * \def TEXT_MESSAGE_TYPE
 * \brief An integer representing a text message
 *
 * Content of packets of this type must contain :
 *  - the text message : (MSG_MAX_LENGTH + 1) bytes long
 *  - the username of the user who sent to message : (USERNAME_MAX_LENGTH + 1) bytes long
 */
#define TEXT_MESSAGE_TYPE 2

/**
 * \def DEFINE_USERNAME_MESSAGE_TYPE
 * \brief An integer representing a message meant to define the client username
 *
 * Content of packets of this type must contain :
 *  - the new username of client : (USERNAME_MAX_LENGTH + 1) bytes long
 */
#define DEFINE_USERNAME_MESSAGE_TYPE 3

/**
 * \def SERVER_ERROR_MESSAGE_TYPE
 * \brief An integer representing an error message sent by server
 *
 * Content of packets of this type must contain :
 *  - the message sent by the server : (MSG_MAX_LENGTH + 1) bytes long
 */
#define SERVER_ERROR_MESSAGE_TYPE 4

/**
 * \def USERNAME_CHANGED_MESSAGE_TYPE
 * \brief An integer representing a message meant to notify clients a client changed its username
 *
 * Content of packets of this type must contain:
 *  - the old username: (USERNAME_MAX_LENGTH + 1) bytes long
 *  - the new username: (USERNAME_MAX_LENGTH + 1) bytes long
 */
#define USERNAME_CHANGED_MESSAGE_TYPE 5

/**
 * \def CLIENT_FILE_MESSAGE_TYPE
 * \brief An integer representing a message meant to notify the server that a client want to send a file
 *
 * Content of packets of this type must contain :
 *  - the file size
 */
#define CLIENT_FILE_MESSAGE_TYPE 6

/**
 * \def SERVEUR_FILE_MESSAGE_TYPE
 * \brief An integer representing a message meant to notify a client that the serveur accept or refuse the file
 *
 * Content of packets of this type must contain :
 *  - the file id
 */
#define SERVEUR_FILE_MESSAGE_TYPE 7

/**
 * \def FILE_MESSAGE_TYPE
 * \brief An integer representing a file message
 *
 * Content of packets of this type must contain :
 *  - the file id
 *  - the file content
 */
#define FILE_MESSAGE_TYPE 8

#endif //C_CHAT_CONSTANTS_H
