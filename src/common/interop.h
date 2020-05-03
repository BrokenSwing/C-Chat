/**
 * \file interop.h
 * \brief A file defining useful macros that indicate if we're on Windows or on POSIX-compliant OS
 *
 * Defines IS_POSIX : 0 if current OS is not POSIX-compliant, else 1
 * Defines IS_WINDOWS : 0 if current OS is not Windows, else 1
 */

#ifndef C_CHAT_INTEROP_H
#define C_CHAT_INTEROP_H

#if defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__linux__)
#define IS_POSIX 1
#else
#define IS_POSIX 0
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS)
#define IS_WINDOWS 1
#else
#define IS_WINDOWS 0
#endif

#endif //C_CHAT_INTEROP_H
