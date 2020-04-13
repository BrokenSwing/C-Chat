#ifndef C_CHAT_CONSOLE_COLOR_H
#define C_CHAT_CONSOLE_COLOR_H

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

#if defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__linux__)

#include <stdio.h>

#define FG_RED 31
#define FG_YELLOW 33
#define FG_WHITE 39

void setTextColor(unsigned int colorCode) {
    printf("\033[%dm", colorCode);
}

#elif defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS)

#ifndef WIN32_LEAN_AND_MEAN
#include <windows.h>
#define WIN32_LEAN_AND_MEAN
#endif

#define FG_RED FOREGROUND_RED | FOREGROUND_INTENSITY
#define FG_YELLOW FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define FG_WHITE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY

void setTextColor(unsigned int colorCode) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
}

#endif

// Implementing here so macros got defined an have associated value
void resetColor() {
    setTextColor(FG_WHITE);
}

#endif //C_CHAT_CONSOLE_COLOR_H
