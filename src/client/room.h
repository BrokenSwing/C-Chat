#ifndef C_CHAT_ROOM_H
#define C_CHAT_ROOM_H

/**
 * \brief Computes the remaining part of the command that aims to create a room.
 *
 * It tries to extract room name and room description from the given command.
 * Then sends a packet to the server to create the room.
 *
 * \param command The part of the command containing the name and the description of the room to create
 */
void createRoom(const char* command);

/**
 * \brief Sends a packet to the server to join the room with the given name.
 *
 * The real room name considered is the first word of the passed argument.
 *
 * \param command The name of the room to join
 */
void joinRoom(const char* roomName);

/**
 * \brief Sends a packet to the server to leave the current room.
 */
void leaveRoom();

/**
 * \brief Sends a packet to the server to list all existing rooms.
 */
void listRooms();

#endif //C_CHAT_ROOM_H
