/**
 * \file ui.c
 * \brief Implementation for ui.h functions
 *
 * This implementation displays received messages to the console.
 */

// This file is a mess

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "console-color.h"
#include "../common/synchronization.h"
#include <math.h>

#define PROMPT_MAX_LENGTH 50
#define QUERYING 1
#define INPUT_STORED 2
static short inputState;
static char promptBuffer[PROMPT_MAX_LENGTH];
static Mutex inputStateMutex;

void _beginUserInput();
void _endUserInput();
void _storeUserInput();
void _restoreUserInput();

#if defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__linux__)

void _beginUserInput() {
    inputState = QUERYING;
}

void _endUserInput() {
    printf("\033[A\33[2K\r");
    inputState = 0;
}

void _storeUserInput() {
    if (inputState & QUERYING) {
        printf("\33[2K\r");
        inputState |= INPUT_STORED;
    }
}

void _restoreUserInput() {
    if (inputState & INPUT_STORED) {
       inputState = (inputState ^ 0) ^ INPUT_STORED;
       printf("%s", promptBuffer);
    }
}

#elif defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS)

static COORD cursorCoord;

void _beginUserInput() {
    inputState = QUERYING;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    GetConsoleScreenBufferInfo(handle, &screenBufferInfo);
    cursorCoord = screenBufferInfo.dwCursorPosition;
}

void _endUserInput() {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(handle, cursorCoord);
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    GetConsoleScreenBufferInfo(handle, &screenBufferInfo);
    printf("\r");
    for (int i = 1; i < screenBufferInfo.dwSize.X; i++) {
        printf(" ");
    }
    printf("\r");
    inputState = 0;
}

void _storeUserInput() {
    if (inputState & QUERYING) {
        _endUserInput();
        inputState = INPUT_STORED | QUERYING;
    }
}

void _restoreUserInput() {
    if (inputState & INPUT_STORED) {
        short currentState = inputState;
        _beginUserInput();
        inputState = (currentState ^ 0) ^ INPUT_STORED;
        printf("%s", promptBuffer);
    }
}

#endif

void ui_init() {
    inputStateMutex = createMutex();
}

void ui_cleanUp() {
    destroyMutex(inputStateMutex);
}

void ui_reset() {
    acquireMutex(inputStateMutex);
    {
        _storeUserInput();
        inputState = 0;
    }
    releaseMutex(inputStateMutex);
}

void ui_getUserInput(const char* prompt, char* buffer, int buffer_size) {
    do {
        // Modify input state
        acquireMutex(inputStateMutex);
        {
            _beginUserInput();
            // Keep prompt in memory for future prompt restoration
            memcpy(promptBuffer, prompt, min(strlen(prompt), PROMPT_MAX_LENGTH));
        }
        releaseMutex(inputStateMutex);

        printf("%s", promptBuffer);
        fgets(buffer, buffer_size, stdin);
        if ((strlen(buffer) - 1) == 0) { // If user entered an empty message.
            // Modify input state
            acquireMutex(inputStateMutex);
            {
                _endUserInput();
            }
            releaseMutex(inputStateMutex);

            ui_errorMessage("You can't send an empty message.");
        }
    } while((strlen(buffer) - 1) == 0);

    // Modify input state
    acquireMutex(inputStateMutex);
    {
        _endUserInput();
    }
    releaseMutex(inputStateMutex);

    // Trim carriage return
    buffer[strlen(buffer) - 1] = '\0';
}

void ui_messageReceived(const char* sender, const char* message) {
    acquireMutex(inputStateMutex);
    {
        _storeUserInput();
    }
    releaseMutex(inputStateMutex);

    printf("[%s] %s\n", sender, message);

    acquireMutex(inputStateMutex);
    {
        _restoreUserInput();
    }
    releaseMutex(inputStateMutex);
}

void ui_informationMessage(const char* message) {
    acquireMutex(inputStateMutex);
    {
        _storeUserInput();
    }
    releaseMutex(inputStateMutex);

    setTextColor(FG_YELLOW);
    printf("%s\n", message);
    resetColor();

    acquireMutex(inputStateMutex);
    {
        _restoreUserInput();
    }
    releaseMutex(inputStateMutex);
}

void ui_errorMessage(const char* message) {
    acquireMutex(inputStateMutex);
    {
        _storeUserInput();
    }
    releaseMutex(inputStateMutex);

    setTextColor(FG_RED);
    printf("%s\n", message);
    resetColor();

    acquireMutex(inputStateMutex);
    {
        _restoreUserInput();
    }
    releaseMutex(inputStateMutex);
}