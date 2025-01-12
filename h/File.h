#pragma once
#include "TypesAndConstants.h"

class File {
public:
    /**
    * @brief  Creates a file or directory if doesn't exist.
    * If the file exists, it will take an OFT entry.
    * Note that if the directory exists and `open` is called, it will raise an exception.
    * @param path path name
    */
    File(const char *path);

    /**
    * @brief Sets file cursor to \p pos .
    * @param pos new cursor position
    * @return <0 if error
    */
    int seek(uint16_t pos);

    /**
     * @brief Reads \p count number of characters starting from file cursor,
     * and copies to \p data. If \p count <0, reads until the end of the file.
     * @param count maximum number of characters to read
     * @param data buffer
     * @return number of characters read or <0 if error
     */
    int read(char *data, size_t count);

    /**
     * @brief Writes \p count characters from buffer \p data to file.
     * @param count number of characters to write
     * @param data buffer
     * @return number of characters written or <0 if error
     */
    int write(const char *data, size_t count);

    /**
     * @brief Changes the name of a file or directory.
     * @param path relative or absolute path to the file or directory
     * @param name new file name
     * @return <0 if error
     */
    static int rename(const char *path, const char *name);

    /**
     * @brief Deletes a file or directory.
     * @param path relative or absolute path to the file or directory
     * @return <0 if error
     */
    static int remove(const char *path);

    /**
     * @brief Returns the cursor to the end of the file through.
     * Returned number is the number of characters in the file.
     * @return <0 if error
     */
    int getEOF() const;

    /**
     * @brief Returns current cursor position.
     * @param cursor returned cursor
     * @return <0 if error
     */
    int getCursor() const;
    
    /**
     * @brief Closes the file.
     */
    ~File();

private:

    /**
     * @brief Saves cached FAT structures to disk and releases OFT entry taken by \p file.
     * @return 1 if already closed or 0 if just closed
     */
    int close();

    bool closed = false;
    FHANDLE handle;
};

