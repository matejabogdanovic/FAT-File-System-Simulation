#pragma once
#include "FAT.h"
#include "Inode.h"

class DirectorySystem {
public:
    /**
     * @brief Resets root block to contain only root folder. Clears \b just the root block.
     */
    void clearRoot();

    /**
     * @brief Prints file system tree starting from root directory.
     */
    void printTree();

    static DirectorySystem &get();

    ~DirectorySystem();

private:
    friend class File;

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
     * @brief Initializes FAT. Allocates directory tree starting from the root directory.
     * @return <0 if error
     */
    int init();

    Inode *loadTree(adisk_t blk); // should be done with block + offset

    /**
     * @brief Links FCB of \p prev to point to FCB block of \p buf . Updates disk.
     * //
     */
    void linkAndWriteFCBs(bool parent, FileControlBlock::fcb_t buf, fat_entry_t fcb_block, Inode *prev);

    Inode *root;
    bool initialized;
    OpenFilesTable oft;
};



