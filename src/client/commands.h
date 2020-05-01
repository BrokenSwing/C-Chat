#ifndef C_CHAT_COMMANDS_H
#define C_CHAT_COMMANDS_H

#include "ui.h"
#include <string.h>

#define COMMAND_HANDLER(NAME, commands) \
void NAME##Handler (const char* command) { \
    commands; \
    ui_errorMessage("Unknown command"); \
}

#define COMMAND(NAME, usage, next) \
if ( \
    strncmp(#NAME, command, strlen(#NAME)) == 0 && strlen(#NAME) == strlen(command) || \
    strncmp(#NAME, command, strlen(#NAME)) == 0 && strncmp(" ", command + strlen(#NAME), 1) == 0 \
) {\
    command = command + strlen(#NAME); \
    int commandLength##NAME = strlen(command); \
    while (commandLength##NAME > 0 && command[0] == ' ') { \
        command += 1; \
        commandLength##NAME = strlen(command); \
    } \
    next; \
    ui_errorMessage(usage); \
    return;\
};

#endif //C_CHAT_COMMANDS_H
