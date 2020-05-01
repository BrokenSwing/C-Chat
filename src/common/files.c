#include "files.h"
#include "interop.h"

#if IS_POSIX

#include <sys/stat.h>
#include <sys/types.h>

FileInfo files_getInfo(const char* filename) {
    struct stat st;

    FileInfo info;
    if (stat(filename, &st) == 0) {
        info.isDirectory = S_ISDIR(st.st_mode);
        if (info.isDirectory) {
            info.size = st.st_size;
        } else {
            info.size = 0;
        }
    } else {
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
    info.isDirectory = (wInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0;
    if (!info.isDirectory) {
        LARGE_INTEGER size;
        size.LowPart = wInfo.nFileSizeLow;
        size.HighPart = wInfo.nFileSizeHigh;
        info.size = size.QuadPart;
    } else {
        info.size = 0;
    }

    return info;
}

#endif
