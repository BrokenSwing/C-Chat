#include "files.h"
#include "interop.h"
#include <stdio.h>

#if IS_POSIX

#include <sys/stat.h>
#include <sys/types.h>

FileInfo files_getInfo(const char* filename) {
    struct stat st;

    FileInfo info;
    if (stat(filename, &st) == 0) {
        info.exists = 1;
        info.isDirectory = S_ISDIR(st.st_mode);
        if (info.isDirectory) {
            info.size = st.st_size;
        } else {
            info.size = 0;
        }
    } else {
        info.exists = 0;
        info.isDirectory = 0;
        info.size = 0;
    }

    return info;
}

#elif IS_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

FileInfo files_getInfo(const char* filename) {
    WIN32_FILE_ATTRIBUTE_DATA wInfo;
    GetFileAttributesEx(filename, GetFileExInfoStandard, &wInfo);

    FileInfo info;
    info.exists = 1;
    info.isDirectory = (wInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0;
    if (!info.isDirectory) {
        LARGE_INTEGER size;
        size.LowPart = wInfo.nFileSizeLow;
        size.HighPart = wInfo.nFileSizeHigh;
        info.size = size.QuadPart;
        if (info.size < 0) {
            info.size = 0;
            info.exists = 0;
        }
    } else {
        info.size = 0;
    }

    return info;
}

unsigned long files_readFile(const char* filename, char* contentBuffer, unsigned long bufferSize) {
    FileInfo info = files_getInfo(filename);
    if (info.isDirectory) {
        return -1;
    }
    HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (GetLastError() == ERROR_FILE_NOT_FOUND) {
        printf("Unable to open file: %s\n", filename);
        CloseHandle(file);
        return -1;
    }

    unsigned long readCount;
    if(!ReadFile(file, contentBuffer, bufferSize, &readCount, NULL)) {
        printf("Unable to read file: %s\n", filename);
        printf("Error code: %ld\n", GetLastError());
        CloseHandle(file);
        return -1;
    }
    CloseHandle(file);

    return readCount;
}

unsigned long files_writeFile(const char* filename, const char* contentBuffer, unsigned long bufferSize) {
    FileInfo info = files_getInfo(filename);
    if (info.exists && info.isDirectory) {
        return -1;
    }
    HANDLE file = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0) {
        printf("Unable to open file: %s\n", filename);
        printf("Error code: %ld", GetLastError());
        CloseHandle(file);
        return -1;
    }

    unsigned long wroteCount;
    if (!WriteFile(file, contentBuffer, bufferSize, &wroteCount, NULL)) {
        printf("Unable to write file: %s\n", filename);
        printf("Error code: %ld\n", GetLastError());
        CloseHandle(file);
        return -1;
    }
    CloseHandle(file);

    return wroteCount;
}

#endif
