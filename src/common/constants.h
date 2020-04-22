/**
 * \file constants.h
 * \brief Defines constants used on client and server (used for consistency)
 */

#ifndef C_CHAT_CONSTANTS_H
#define C_CHAT_CONSTANTS_H

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
 * \def MESSAGE_TYPE_OVERHEAD
 * \brief The number of bytes the type of the message is
 */
#define MESSAGE_TYPE_OVERHEAD 1

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

#endif //C_CHAT_CONSTANTS_H
