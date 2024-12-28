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


    int remove(const char *path, FILE_EXT ext);

    /**
     * @brief Prints file system tree starting from root directory.
     */
    void printTree() const;

    /**
     * @brief
     * @return current working directory name
     */
    const char *getWorkingDirectoryName();

    const char *setWDtoParent();

    int setWDto(char *path);

    void listWDFiles() const;

    ~FileSystem();

private:
    friend class File;

    friend class Console;

    FileSystem();

    int getFileName(char *path, size_t *path_len, char *file_name, FILE_EXT extension);

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

    inline Inode *getWorkingDirectory() const;

    int setWorkingDirectory(Inode *working);

    inline static block_cnt_t sizeToBlocks(size_t size);

    Inode *root = nullptr;
    Inode *working = nullptr;
    bool initialized = false;
    OpenFilesTable oft;
};



