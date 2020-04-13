/**
 * \file sockets.c
 * \brief API for sockets.
 * 
 * Implementation of "sockets.h" file.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "sockets.h"

#define CLIENTS_BACKLOG 5
#if defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__linux__)

    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <inttypes.h>

    struct UnixSocketInfo {
        int socket;
    };

    SocketInfo createClientSocket(const char* ipAddress, const char* port) {
        int s = socket(PF_INET, SOCK_STREAM, 0);
        if (s == -1) {
            printf("Unable to create client socket.\n");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in adS;
        adS.sin_family = AF_INET;
        adS.sin_port = htons((int)strtol(port, NULL, 10));
        inet_pton(AF_INET, ipAddress, &adS.sin_addr);
        socklen_t lgA = sizeof(struct sockaddr_in);

        if (connect(s, (struct sockaddr *)&adS, lgA) == -1) {
            printf("Unable to connect to server.\n");
            exit(EXIT_FAILURE);
        }

        struct UnixSocketInfo *socketInfo = malloc(sizeof(struct UnixSocketInfo));
        socketInfo->socket = s;

        SocketInfo ret;
        ret.info = socketInfo;

        return ret;
    }

    SocketInfo createServerSocket(const char* port) {
        int s = socket(PF_INET, SOCK_STREAM, 0);
        if (s == -1) {
            printf("Unable to create server socket.\n");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in ad;
        ad.sin_family = AF_INET;
        ad.sin_addr.s_addr = INADDR_ANY;
        ad.sin_port = htons((int)strtol(port, NULL, 10));

        if (bind(s, (struct sockaddr*)&ad, sizeof(ad)) == -1)
        {
            printf("Unable to bind server socket.\n");
            close(s);
            exit(EXIT_FAILURE);
        }

        if (listen(s, CLIENTS_BACKLOG) == -1)
        {
            printf("Unable to listen for incoming clients.\n");
            close(s);
            exit(EXIT_FAILURE);
        }

        struct UnixSocketInfo* socketInfo = malloc(sizeof(struct UnixSocketInfo));
        socketInfo->socket = s;

        SocketInfo ret;
        ret.info = socketInfo;

        return ret;
    }

    SocketInfo acceptClient(SocketInfo serverSocket) {
        struct UnixSocketInfo *socketInfo = serverSocket.info;

        struct sockaddr_in adClient;
        socklen_t lgA = sizeof(struct sockaddr_in);
        int clientSocket = accept(socketInfo->socket, (struct sockaddr*)&adClient, &lgA);
        if (clientSocket == -1) {
            printf("Unable to accept client connection.\n");
            close(socketInfo->socket);
            exit(EXIT_FAILURE);
        }

        struct UnixSocketInfo *clientInfo = malloc(sizeof(struct UnixSocketInfo));
        clientInfo->socket = clientSocket;

        SocketInfo ret;
        ret.info = clientInfo;

        return ret;
    }

    int receiveFrom(SocketInfo clientSocket, char* buffer, unsigned int bufferSize) {
        struct UnixSocketInfo *socketInfo = clientSocket.info;

        int callSuccess = recv(socketInfo->socket, buffer, bufferSize, 0);
        if (callSuccess == 0) {
            printf("Connection closed.\n");
        } else if(callSuccess < 0) {
            printf("Unable to received data.\n");
            close(socketInfo->socket);
            exit(EXIT_FAILURE);
        }

        return callSuccess;
    }

    int sendTo(SocketInfo clientSocket, const char* buffer, unsigned int bufferSize) {
        struct UnixSocketInfo *socketInfo = clientSocket.info;

        int callSuccess = send(socketInfo->socket, buffer, bufferSize, 0);
        if (callSuccess == 0) {
            printf("Connection closed.\n");
        } else if (callSuccess < 0) {
            printf("Unable to send data through socket.\n");
            close(socketInfo->socket);
            exit(EXIT_FAILURE);
        }

        return callSuccess;
    }

    void closeSocket(SocketInfo socket) {
        struct UnixSocketInfo *socketInfo = socket.info;
        close(socketInfo->socket);
    }

    void cleanUp() {
        // Nothing to cleanup under Unix
    }

#elif defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS)

	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock2.h>
    #include <ws2tcpip.h>

	#pragma comment (lib, "Ws2_32.lib")
	#pragma comment (lib, "Mswsock.lib")
	#pragma comment (lib, "AdvApi32.lib")

    #define INIT_WIN_LIB() if (!winLibInitialized) {    \
                                initialize();           \
                                winLibInitialized = 1;  \
                            }

    static int winLibInitialized = 0;

    struct WinSocketInfo {
        SOCKET socket;
    };

    void initialize() {
        WSADATA wsaData;

        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            printf("Windows WebSocket API startup failed. Error code : %d\n", result);
            exit(EXIT_FAILURE);
        }
    }

    SocketInfo createClientSocket(const char* ipAdress, const char* port) {
        INIT_WIN_LIB();
        struct addrinfo *result = NULL,
                        hints;
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        int callSuccess = getaddrinfo(ipAdress, port, &hints, &result);
        if (callSuccess != 0) {
            printf("Unable to get address for %s:%s. Error code : %d\n", ipAdress, port, callSuccess);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        SOCKET ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("Error at client socket creation. Error code : %d\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        int callResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
        if (callResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
        }

        freeaddrinfo(result);

        if (ConnectSocket == INVALID_SOCKET) {
            printf("Unable to connect to server.\n");
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        struct WinSocketInfo* info = malloc(sizeof(struct WinSocketInfo));
        info->socket = ConnectSocket;

        SocketInfo ret;
        ret.info = info;

        return ret;
    }

    SocketInfo createServerSocket(const char* port) {
        INIT_WIN_LIB();
        struct addrinfo *result = NULL,
                         hints;

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        int callSuccess = getaddrinfo(NULL, port, &hints, &result);
        if (callSuccess != 0) {
            printf("Unable to get address for localhost:%s. Error code : %d\n", port, callSuccess);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            printf("Error at server socket creation. Error code : %d\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        callSuccess = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        if (callSuccess == SOCKET_ERROR) {
            printf("Unable to bind server socket. Error code : %d\n", WSAGetLastError());
            freeaddrinfo(result);
            closesocket(ListenSocket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        freeaddrinfo(result);

        if (listen(ListenSocket, CLIENTS_BACKLOG) == SOCKET_ERROR) {
            printf("Unable to listen for incoming clients. Error code : %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        struct WinSocketInfo* info = malloc(sizeof(struct WinSocketInfo));
        info->socket = ListenSocket;

        SocketInfo ret;
        ret.info = info;

        return ret;
    }

    SocketInfo acceptClient(SocketInfo info) {
        struct WinSocketInfo *socketInfo = info.info;

        SOCKET ClientSocket = accept(socketInfo->socket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("Unable to accept client connection. Error code : %d\n", WSAGetLastError());
            closesocket(socketInfo->socket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        struct WinSocketInfo *clientInfo = malloc(sizeof(struct WinSocketInfo));
        clientInfo->socket = ClientSocket;

        SocketInfo ret;
        ret.info = clientInfo;
        return ret;
    }

    int receiveFrom(SocketInfo clientSocket, char* buffer, unsigned int bufferSize) {
        struct WinSocketInfo *socketInfo = clientSocket.info;

        int callSuccess = recv(socketInfo->socket, buffer, bufferSize, 0);
        if (callSuccess == 0) {
            printf("Connection closed.\n");
        } else if(callSuccess < 0) {
            printf("Unable to received data. Error code : %d\n", WSAGetLastError());
            closesocket(socketInfo->socket);
            exit(EXIT_FAILURE);
        }

        return callSuccess;
    }

    int sendTo(SocketInfo clientSocket, const char* buffer, unsigned int bufferSize) {
        struct WinSocketInfo *socketInfo = clientSocket.info;

        int callSuccess = send(socketInfo->socket, buffer, bufferSize, 0);
        if (callSuccess == SOCKET_ERROR) {
            printf("Unable to send data through socket. Error code : %d\n", WSAGetLastError());
            closesocket(socketInfo->socket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        return callSuccess;
    }

    void closeSocket(SocketInfo socket) {
        struct WinSocketInfo *socketInfo = socket.info;
        shutdown(socketInfo->socket, SD_BOTH);
        closesocket(socketInfo->socket);
    }

    void cleanUp() {
        WSACleanup();
    }

#endif