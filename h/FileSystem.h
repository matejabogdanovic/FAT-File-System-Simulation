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

    int remove(pathname_t path, FILE_EXT ext);

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

    void listWDFiles() const;

    ~FileSystem();

private:
    friend class File;

    FileSystem();

    /**
     * @brief
     * @param path
     * @param extension
     * @param size
     * @return
     */
    FHANDLE open(pathname_t path, FILE_EXT extension, size_t size);

    /**
     * @brief Saves cached FAT structures to disk and releases OFT entry taken by \p file.
     * @param file
     * @return
     */
    int close(FHANDLE file);

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



