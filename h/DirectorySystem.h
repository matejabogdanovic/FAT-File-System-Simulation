#pragma once
#include "FAT.h"
#include "Inode.h"

class DirectorySystem {
public:
    /**
     * @brief Resets root block to contain only root folder. Clears \b just the root block.
     */
    static void clearRoot();

    ~DirectorySystem();

private:
    friend class File;

    /**
     * @brief
     * @param name
     * @param extension
     * @param size
     * @return
     */
    static FHANDLE open(filename_t name, FILE_EXT extension, size_t size);

    /**
     * @brief Saves cached FAT structures to disk and releases OFT entry taken by \p file.
     * @param file
     * @return
     */
    static int close(FHANDLE file);

    /**
     * @brief Initializes FAT. Allocates directory tree starting from the root directory.
     */
    static void init();

    static Inode *root;
    static bool initialized;
    static OpenFilesTable oft;
};



