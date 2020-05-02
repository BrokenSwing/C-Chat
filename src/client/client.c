/**
 * \file client.c
 * \brief Client file.
 * 
 * Allows connection to a server and sending messages to another client.
 * 
 */

#include <string.h>
#include <stdlib.h>
#include "../common/sockets.h"
#include <string.h>
#include "ui.h"
#include "../common/threads.h"
#include "../common/constants.h"
#include "client.h"
#include "commands.h"

static SocketInfo clientSocket;

THREAD_ENTRY_POINT sendMessage(void* data) {
    char buffer[MESSAGE_TYPE_OVERHEAD + MSG_MAX_LENGTH + 1];
    buffer[0] = TEXT_MESSAGE_TYPE;
    while(1) {
        ui_getUserInput("Your message : ", buffer + MESSAGE_TYPE_OVERHEAD, MSG_MAX_LENGTH);
        if (strncmp("/", buffer + MESSAGE_TYPE_OVERHEAD, 1) == 0) { // Is a command
            commandHandler(buffer + MESSAGE_TYPE_OVERHEAD + 1);
        } else {
            sendTo(clientSocket, buffer, MESSAGE_TYPE_OVERHEAD + strlen(buffer + MESSAGE_TYPE_OVERHEAD));
        }
    }
}

COMMAND_HANDLER(command,
COMMAND(file, "Usage: /file <send | receive>",
        COMMAND(send, "Usage: /file send <filepath>",
            if (strlen(command) > 0) {
                    ui_informationMessage("Command send detected");
                    return;
                }
            )
            COMMAND(receive, "Usage: /file receive <id>",
            if (strlen(command) > 0) {
                    ui_informationMessage("Command receive detected");
                    return;
                }
            )
        )
        COMMAND(nick, "Usage: /nick <username>",
            if (strlen(command) > 0) {
                setUsername(command);
                return;
            }
        )
)

void receiveMessages() {
    char buffer[MESSAGE_TYPE_OVERHEAD + (MSG_MAX_LENGTH + 1) + (USERNAME_MAX_LENGTH + 1)];
    int bytesCount;
    do {
        bytesCount = receiveFrom(clientSocket, buffer, MESSAGE_TYPE_OVERHEAD + (MSG_MAX_LENGTH + 1) + (USERNAME_MAX_LENGTH + 1));
        if (bytesCount > 0) {
            char messageType = buffer[0];
            switch (messageType) {
                case TEXT_MESSAGE_TYPE:
                    ui_messageReceived(buffer + MSG_MAX_LENGTH + 2, buffer + MESSAGE_TYPE_OVERHEAD);
                    break;
                case JOIN_MESSAGE_TYPE:
                    ui_joinMessage(buffer + 1);
                    break;
                case LEAVE_MESSAGE_TYPE:
                    ui_leaveMessage(buffer + 1);
                    break;
                case SERVER_ERROR_MESSAGE_TYPE:
                    ui_errorMessage(buffer + 1);
                    break;
                case USERNAME_CHANGED_MESSAGE_TYPE:
                    char changeMessage[USERNAME_MAX_LENGTH * 2 + 25 + 1];
                    unsigned int oldUsernameLength = strlen(buffer + MESSAGE_TYPE_OVERHEAD);
                    unsigned int newUsernameLength = strlen(buffer + MESSAGE_TYPE_OVERHEAD + (USERNAME_MAX_LENGTH + 1));
                    memcpy(changeMessage, buffer + MESSAGE_TYPE_OVERHEAD, oldUsernameLength);
                    memcpy(changeMessage + oldUsernameLength, " changed its username to ", 25);
                    memcpy(
                            changeMessage + oldUsernameLength + 25,
                            buffer + MESSAGE_TYPE_OVERHEAD + (USERNAME_MAX_LENGTH + 1),
                            newUsernameLength
                    );
                    changeMessage[oldUsernameLength + 25 + newUsernameLength] = '\0';
                    ui_informationMessage(changeMessage);
                    break;
                default:
                    break;
            }
        }
    } while (bytesCount > 0);
}

void pickUsername() {
    int success = 0;
    do {
        char username[50];
        ui_getUserInput("Your username : ", username, 20);
        int bytesReceived = sendTo(clientSocket, username, strlen(username));

        if (bytesReceived < 0) {
            ui_informationMessage("Connection with server lost. Exiting.");
            closeSocket(&clientSocket);
            cleanUp();
            exit(EXIT_FAILURE);
        }

        bytesReceived = receiveFrom(clientSocket, username, 100);
        if (bytesReceived > 0) {
            username[bytesReceived] = '\0';
            int passwordOk = username[0] == 'O' && username[1] == 'k';
            if (passwordOk) {
                success = 1;
            } else {
                ui_informationMessage(username);
            }
        } else {
            ui_informationMessage("Connection with server lost. Exiting.");
            closeSocket(&clientSocket);
            cleanUp();
            exit(EXIT_FAILURE);
        }
    } while (!success);
}

void setUsername(const char* newUsername) {
    char changeUsernamePacket[MESSAGE_TYPE_OVERHEAD + (USERNAME_MAX_LENGTH + 1)];
    changeUsernamePacket[0] = DEFINE_USERNAME_MESSAGE_TYPE;
    memcpy(changeUsernamePacket + MESSAGE_TYPE_OVERHEAD, newUsername, USERNAME_MAX_LENGTH);
    changeUsernamePacket[MESSAGE_TYPE_OVERHEAD + USERNAME_MAX_LENGTH] = '\0';

    sendTo(clientSocket, changeUsernamePacket, MESSAGE_TYPE_OVERHEAD + (USERNAME_MAX_LENGTH + 1));
}

/**
 * \brief Program entry.
 * 
 * \return EXIT_SUCCESS - normal program termination.
 */
int main() {
    ui_init();
    clientSocket = createClientSocket("127.0.0.1", "27015");
    ui_informationMessage("Hi, you're connected to server !");

    pickUsername();

    Thread senderThread = createThread(sendMessage, NULL);

    receiveMessages();

    destroyThread(&senderThread);

    ui_reset();

    ui_informationMessage("End of chat.");
    ui_cleanUp();

    closeSocket(&clientSocket);
    cleanUp();
    return EXIT_SUCCESS;
}