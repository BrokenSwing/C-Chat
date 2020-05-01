#include "../common/sockets.h"
#include "../common/constants.h"
#include "../common/threads.h"

#ifndef C_CHAT_CLIENT_H
#define C_CHAT_CLIENT_H

/**
 * \def NUMBER_CLIENT_MAX
 * \brief Maximum of simultaneous connected clients
 */
#define NUMBER_CLIENT_MAX 10

/**
 * \class Client
 * \brief A type representing a connected client
 */
typedef struct Client {
    /** The socket from server to the client */
    SocketInfo socket;
    /** A buffer meant to contain client username */
    char username[USERNAME_MAX_LENGTH + 1];
    /**
     * A short indicating whether or not, the client joined discussion.
     * Equal to 0 if client isn't in the discussion, else 1.
     */
    short joined;
    /** Thread relaying messages sent by user */
    Thread thread;
} Client;

/**
 * \brief Scan clients slots for an available slot.
 *
 * Iterates over clients slots to find an empty slot.
 *
 * \return -1 if no empty slot was found, else an available slot id
 */
int scanForFreeSocketSlot();

/**
 * \brief Catch interrupt signal.
 *
 * \param signal Incoming signal
 */
void handleServerClose(int signal);

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

/**
 * \brief Verifies if the end of the communication is requested.
 *
 * \param buffer A message.
 * \return 1 if the message is "fin", else 0.
 */
int receivedEndMessage(const char* buffer);

/**
 * \brief Broadcasts a message to every clients
 *
 * @param buffer The message to broacast
 * @param size The message length
 */
void broadcast(const char* buffer, int size);

/**
 * \brief Relay messages sent by given client to all known clients.
 *
 * Waits for an incoming message, and broadcast it (in a correctly formed relayed message) to all
 * known clients.
 *
 * @param client The client to wait messages from
 */
void relayClientMessages(Client* client);

#endif //C_CHAT_CLIENT_H
