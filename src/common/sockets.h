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
 * \param ipAddress Serveur IP.
 * \param port Serveur port.
 * \return A SocketInfo struct which contains the client socket.
*/
SocketInfo createClientSocket(const char* ipAddress, const char* port);

/**
 * \brief Creates a socket meant to receive connection from clients sockets.
 * 
 * \param port Serveur port.
 * \return A SocketInfo struct which contains the serveur socket.
*/
SocketInfo createServerSocket(const char* port);

/**
 * \brief Waits for a client to connect to the given server socket. Returns the socket for the connected client.
 * 
 * \param serverSocket The server socket.
 * \return A SocketInfo struct which contains the socket for the connected client.
*/
SocketInfo acceptClient(SocketInfo serverSocket);

/**
 * \brief Receives through from the given socket.
 * 
 * \param clientSocket The given socket.
 * \param buffer Contains the received message.
 * \param bufferSize Buffer size.
 * \return Number of received bytes or ERROR.
*/
int receiveFrom(SocketInfo clientSocket, char* buffer, unsigned int bufferSize);

/**
 * \brief Sends data through the given socket.
 * 
 * \param clientSocket The given socket.
 * \param buffer Contains the message.
 * \param bufferSize Buffer size.
 * \return Number of sent bytes or ERROR.
*/
int sendTo(SocketInfo clientSocket, const char* buffer, unsigned int bufferSize);

/**
 * \brief Closes the given socket.
 * 
 * \param socket The given socket.
*/
void closeSocket(SocketInfo socket);

/**
 * \brief Performs any required resources cleanup. Must be called once all sockets usage is finished.
*/
void cleanUp();