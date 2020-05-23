#include "client-info.h"
#include "../common/constants.h"
#include <string.h>
#include "communication.h"

int validateUsername(const char* username) {
    unsigned int usernameLength = strlen(username);
    return usernameLength > 0 && usernameLength <= USERNAME_MAX_LENGTH;
}

void setClientUsername(Client* client, const char* newUsername) {
    unsigned int newUsernameLength = strlen(newUsername);
    SYNC_CLIENT_WRITE(
        memcpy(client->username, newUsername, newUsernameLength);
        client->username[newUsernameLength] = '\0';
    );
}

void getClientUsername(Client* client, char* dest) {
    SYNC_CLIENT_READ(memcpy(dest, client->username, USERNAME_MAX_LENGTH + 1));
}

void handleUsernameChange(Client* client, struct PacketDefineUsername* packet) {
    if (validateUsername(packet->username)) {
        Packet changedUsernamePacket = NewPacketUsernameChanged;

        getClientUsername(client, changedUsernamePacket.asUsernameChangedPacket.oldUsername);
        setClientUsername(client, packet->username);
        getClientUsername(client, changedUsernamePacket.asUsernameChangedPacket.newUsername);

        if (client->room != NULL) {
            broadcastRoom(&changedUsernamePacket, client->room);
        }

    } else {
        Packet serverErrorPacket = NewPacketServerErrorMessage;
        memcpy(serverErrorPacket.asServerErrorMessagePacket.message, "Invalid username", 17);
        sendPacket(client->socket, &serverErrorPacket);
    }
}