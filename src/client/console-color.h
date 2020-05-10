/**
 * \file console-color.h
 * \brief API to control text color in console.
 *
 */

#ifndef C_CHAT_CONSOLE_COLOR_H
#define C_CHAT_CONSOLE_COLOR_H

#include "../common/interop.h"

/**
 * \brief Sets output text color.
 *
 * \param colorCode The new color for the text (see defined macros)
 */
void setTextColor(unsigned int colorCode);

/**
 * \brief Set output text color to white.
 *
 * We're assuming default text color is white. It isn't the case for all terminals.
 * If you wan't consistency, this function must be called at the start of your program
 * before any console output.
 */
void resetColor();

#if IS_POSIX

#include <stdio.h>

/**
 * \def FG_RED
 * \brief Red text color constant.
 */
#define FG_RED 31
/**
 * \def FG_YELLOW
 * \brief Yellow text color constant.
 */
#define FG_YELLOW 33
/**
 * \def FG_WHITE
 * \brief White text color constant.
 */
#define FG_WHITE 39
/**
 * \def FG_BLUE
 * \brief Blue text color constant.
 */
#define FG_BLUE 96
/**
 * \def FG_GREEN
 * \brief Green text color constant.
 */
#define FG_GREEN 32

void setTextColor(unsigned int colorCode) {
    printf("\033[%dm", colorCode);
}

#elif IS_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#include <windows.h>
#define WIN32_LEAN_AND_MEAN
#endif

/**
 * \def FG_RED
 * \brief Red text color constant.
 */
#define FG_RED FOREGROUND_RED | FOREGROUND_INTENSITY
/**
 * \def FG_YELLOW
 * \brief Yellow text color constant.
 */
#define FG_YELLOW FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
/**
 * \def FG_WHITE
 * \brief White text color constant.
 */
#define FG_WHITE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
/**
 * \def FG_BLUE
 * \brief Blue text color constant.
 */
#define FG_BLUE FOREGROUND_BLUE | FOREGROUND_INTENSITY | FOREGROUND_GREEN
/**
 * \def FG_GREEN
 * \brief Green text color constant.
 */
#define FG_GREEN FOREGROUND_INTENSITY | FOREGROUND_GREEN


void setTextColor(unsigned int colorCode) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
}

#endif

// Implementing here so macros got defined an have associated value
void resetColor() {
    setTextColor(FG_WHITE);
}

#endif //C_CHAT_CONSOLE_COLOR_H
