#ifndef C_CHAT_FILES_H
#define C_CHAT_FILES_H

/**
 * \class FileInfo
 * \brief A class containing information about a file
 */
typedef struct FileInfo {
    /** Equal to 1 if the file exists, else 0 */
    int exists;
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

/**
 * \brief Reads the given file
 *
 * An error can occur if :
 *  - the given file is a directory
 *  - the given file is protected against reading
 *  - the given buffer is too small for the specified size
 *
 * \param filename The name of the file to read
 * \param contentBuffer The buffer to store file content in
 * \param bufferSize The size of the given buffer
 *
 * \return the actually read size or -1 if an error occurred
 */
unsigned long files_readFile(const char* filename, char* contentBuffer, unsigned long bufferSize);

/**
 * \brief Writes the given file
 *
 * An error can occur if :
 *  - the given file is a directory
 *  - the given file is protected against writing
 *  - the given buffer is too small for the specified size
 *
 * \param filename The name of the file to write to
 * \param contentBuffer The buffer to get file content from
 * \param bufferSize The size of the given buffer
 *
 * \return the actually written size or -1 if an error occurred
 */
unsigned long files_writeFile(const char* filename, const char* contentBuffer, unsigned long bufferSize);

#endif //C_CHAT_FILES_H
