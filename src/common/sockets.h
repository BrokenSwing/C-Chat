/**
 * \file sockets.h
 * \brief API for sockets.
 * 
 * A general API to manipulate sockets.
 * 
 */

#ifndef C_CHAT_SOCKETS_H
#define C_CHAT_SOCKETS_H

/**
 * \struct Socket
 * \brief Information of a socket.
 * 
 * To store information of a socket.
 * 
 */
typedef struct Socket {
    void* info;
} Socket;

/**
 * \brief Creates a socket meant to be used to connect to a server socket and initialize a connection with the server.
 * 
 * \param ipAddress The IP address of the server to connect to
 * \param port The port to connect on
 * \return a ready-to-use client socket
*/
Socket createClientSocket(const char* ipAddress, const char* port);

/**
 * \brief Creates a socket meant to receive connection from clients sockets.
 * 
 * \param port The port the server must listen on
 * \return a ready-to-use server socket
*/
Socket createServerSocket(const char* port);

/**
 * \brief Waits for a client to connect to the given server socket. Returns the socket for the connected client.
 * 
 * \param serverSocket The server socket
 * \return a socket to the accepted client
*/
Socket acceptClient(Socket serverSocket);

/**
 * \brief Receives through from the given socket.
 * 
 * \param clientSocket The socket to receive data from
 * \param buffer A buffer to store received data in
 * \param bufferSize The maximum size of data to receive
 * \return the number of bytes received or -1 if an error occurred
*/
int receiveFrom(Socket clientSocket, char* buffer, unsigned int bufferSize);

/**
 * \brief Sends data through the given socket.
 * 
 * \param clientSocket The socket to send a message through
 * \param buffer A buffer containing to data to send
 * \param bufferSize The size of the data to send
 * \return the number of bytes sent or -1 if an error occurred
*/
int sendTo(Socket clientSocket, const char* buffer, unsigned int bufferSize);

/**
 * \brief Closes the given socket.
 * 
 * \param socket The a pointer to the socket to close
*/
void closeSocket(Socket* socket);

/**
 * \brief Performs any required resources cleanup. Must be called once all sockets usage is finished.
*/
void cleanUp();

#endif //C_CHAT_PACKETS_H