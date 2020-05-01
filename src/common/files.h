#ifndef C_CHAT_FILES_H
#define C_CHAT_FILES_H

/**
 * \class FileInfo
 * \brief A class containing information about a file
 */
typedef struct FileInfo {
    /** Equal to 1 if the file is a directory */
    int isDirectory;
    /** File size in bytes. If the file is a directory, this is equal to 0 */
    long long size;
} FileInfo;

/**
 * \brief Retrieves information about the file with the given name
 *
 * @param filename The name of the file to get information of
 * @return the information of the file with the given name
 */
FileInfo files_getInfo(const char* filename);

#endif //C_CHAT_FILES_H
