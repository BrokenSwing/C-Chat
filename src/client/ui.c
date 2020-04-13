#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"

void ui_getUserInput(char* buffer, int buffer_size) {
    do {
        printf("You : ");
        fgets(buffer, buffer_size, stdin);
        if ((strlen(buffer) - 1) == 0) { // If user entered an empty message.
            printf("You can't send an empty message.\n");
        }
    } while((strlen(buffer) - 1) == 0);

    // Trim carriage return
    buffer[strlen(buffer) - 1] = '\0';
}

void ui_messageReceived(const char* sender, const char* message) {
    printf("[%s] %s\n", sender, message);
}

void ui_informationMessage(const char* message) {
    printf("%s\n", message);
}