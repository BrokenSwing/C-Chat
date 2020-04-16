/**
 * \file ui.c
 * \brief Implementation for ui.h functions
 *
 * This implementation displays received messages to the console.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "console-color.h"

static int queryingUserInput = 0;

void _beginUserInput();
void _endUserInput(int properly);

#if defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__linux__)

void _beginUserInput() {
    queryingUserInput = 1;
    printf("You : ");
}

void _endUserInput(int properly) {
    if (properly) {
        printf("\033[A");
    }
    printf("\33[2K");
    queryingUserInput = 0;
}

#elif defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS)

static COORD cursorCoord;

void _beginUserInput() {
    queryingUserInput = 1;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    GetConsoleScreenBufferInfo(handle, &screenBufferInfo);
    cursorCoord = screenBufferInfo.dwCursorPosition;
    printf("You : ");
}

void _endUserInput(int properly) {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(handle, cursorCoord);
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    GetConsoleScreenBufferInfo(handle, &screenBufferInfo);
    printf("\r");
    for (int i = 1; i < screenBufferInfo.dwSize.X; i++) {
        printf(" ");
    }
    printf("\r");
    queryingUserInput = 0;
}

#endif

void ui_getUserInput(char* buffer, int buffer_size) {
    do {
        _beginUserInput();
        fgets(buffer, buffer_size, stdin);
        if ((strlen(buffer) - 1) == 0) { // If user entered an empty message.
            _endUserInput(1);
            printf("You can't send an empty message.\n");
        }
    } while((strlen(buffer) - 1) == 0);
    _endUserInput(1);

    // Trim carriage return
    buffer[strlen(buffer) - 1] = '\0';
}

void ui_messageReceived(const char* sender, const char* message) {
    if (queryingUserInput) {
        _endUserInput(0);
        printf("[%s] %s\n", sender, message);
        _beginUserInput();
    } else {
        printf("[%s] %s\n", sender, message);
    }
}

void ui_informationMessage(const char* message) {
    if (queryingUserInput) {
        _endUserInput(0);
        setTextColor(FG_YELLOW);
        printf("%s\n", message);
        resetColor();
        _beginUserInput();
    } else {
        setTextColor(FG_YELLOW);
        printf("%s\n", message);
        resetColor();
    }
}