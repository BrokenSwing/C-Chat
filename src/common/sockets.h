/**
 * \file sockets.h
 * \brief API for sockets.
 * 
 * A general API to manipulate sockets.
 * 
 */

/**
 * \struct SocketInfo
 * \brief Informations of a socket.
 * 
 * To store informations of a socket.
 * 
 */
typedef struct SocketInfo {
    void* info;
} SocketInfo;

/**
 * \brief Creates a socket meant to be used to connect to a server socket and initialize a connection with the server.
 * 
 * \param ipAddress The IP address of the server to connect to
 * \param port The port to connect on
 * \return a ready-to-use client socket
*/
SocketInfo createClientSocket(const char* ipAddress, const char* port);

/**
 * \brief Creates a socket meant to receive connection from clients sockets.
 * 
 * \param port The port the server must listen on
 * \return a ready-to-use server socket
*/
SocketInfo createServerSocket(const char* port);

/**
 * \brief Waits for a client to connect to the given server socket. Returns the socket for the connected client.
 * 
 * \param serverSocket The server socket
 * \return a socket to the accepted client
*/
SocketInfo acceptClient(SocketInfo serverSocket);

/**
 * \brief Receives through from the given socket.
 * 
 * \param clientSocket The socket to receive data from
 * \param buffer A buffer to store received data in
 * \param bufferSize The maximum size of data to receive
 * \return the number of bytes received or -1 if an error occurred
*/
int receiveFrom(SocketInfo clientSocket, char* buffer, unsigned int bufferSize);

/**
 * \brief Sends data through the given socket.
 * 
 * \param clientSocket The socket to send a message through
 * \param buffer A buffer containing to data to send
 * \param bufferSize The size of the data to send
 * \return the number of bytes sent or -1 if an error occurred
*/
int sendTo(SocketInfo clientSocket, const char* buffer, unsigned int bufferSize);

/**
 * \brief Closes the given socket.
 * 
 * \param socket The a pointer to the socket to close
*/
void closeSocket(SocketInfo* socket);

/**
 * \brief Performs any required resources cleanup. Must be called once all sockets usage is finished.
*/
void cleanUp();