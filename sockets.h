
typedef struct SocketInfo {
    void* info;
} SocketInfo;

/*
    Creates a socket meant to be used to connect to a server socket
    and initialize a connection with the server.
*/
SocketInfo createClientSocket(const char* ipAddress, const char* port);

/*
    Creates a socket meant to receive connection from clients sockets.
*/
SocketInfo createServerSocket(const char* port);

/*
    Waits for a client to connect to the given server socket.
    Returns the socket for the connected client.
*/
SocketInfo acceptClient(SocketInfo serverSocket);

/*
    Receives through from the given socket.
*/
int receiveFrom(SocketInfo clientSocket, char* buffer, unsigned int bufferSize);

/*
    Sends data through the given socket.
*/
int sendTo(SocketInfo clientSocket, const char* buffer, unsigned int bufferSize);

/*
    Closes the given socket.
*/
void closeSocket(SocketInfo socket);

/*
    Performs any required resources cleanup. Must be called once all sockets usage is finished.
*/
void cleanUp();