#pragma once
#include "FAT.h"
#include "Inode.h"

class FileSystem {
public:
    /**
     * @brief
     * @return instance of FileSystem
     */
    static FileSystem &get();

    /**
     * @brief Resets root block to contain only root folder. Clears \b just the root block.
     */
    static void clearRoot(); // temporary public


    int remove(const char *path, FILE_EXT extension);

    /**
     * @brief Prints file system tree starting from root directory.
     */
    void printTree() const;

    /**
     * @brief
     * @return current working directory name
     */
    [[nodiscard]] inline const char *getWorkingDirectoryName() const;

    const char *setWorkingDirectoryToParent();

    int setWorkingDirectory(char *path);

    void listWorkingDirectory() const;

    ~FileSystem();

private:
    friend class File;

    friend class Console;

    FileSystem();

    /**
     * @brief Helper function for finding file name from path.
     * @param path
     * @param extension
     * @param path_len return value
     * @param file_name return value
     * @return
     */
    int getFileName(char *path, FILE_EXT extension, size_t *path_len, char *file_name);

    Inode::Status searchTree(const char *path, FILE_EXT extension,
                             Inode::Status *status, Inode **previous, Inode **logical_parent,
                             Inode **node, char *filename);

    /**
     * @brief
     * @param path
     * @param extension
     * @param size
     * @return
     */
    FHANDLE open(const char *path, FILE_EXT extension, size_t size);

    /**
     * @brief Saves cached FAT structures to disk and releases OFT entry taken by \p file.
     * @param file
     * @return
     */
    int close(FHANDLE file);

    /**

     */
    int close(const char *path, FILE_EXT extension);

    /**
     * @brief Allocates directory tree starting from the root directory.
     * @return <0 if error
     */
    int init();

    static Inode *loadTree(adisk_t blk, Inode *logical_parent = nullptr);

    [[nodiscard]] inline Inode *getWorkingDirectory() const;

    int setWorkingDirectory(Inode *working);

    inline static block_cnt_t sizeToBlocks(size_t size);

    Inode *root = nullptr;
    Inode *working = nullptr;
    OpenFilesTable oft;
};



