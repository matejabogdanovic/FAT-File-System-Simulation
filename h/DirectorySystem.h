#pragma once
#include "FAT.h"
#include "FileControlBlock.h"

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
     * @brief
     * @param file
     * @return
     */
    static int close(FHANDLE file);

    /**
     * @brief Allocates directory tree starting from the root directory.
     */
    static void init();

    /**
     * @brief Can be either a directory or a file. It's determined by FCB extension field.
     */
    struct Inode {

        FileControlBlock::FCB *fcb;
        Inode *left, *right;

        explicit Inode(FileControlBlock::FCB *fcb, Inode *left = nullptr, Inode *right = nullptr);

        /**
         * @brief Creates and allocates one Directory node from \p fcb block.
         * @param fcb
         * @return Allocated node.
         */
        static Inode *make_node(FileControlBlock::fcb_t fcb);

        ~Inode();
    };

    static Inode *root;
    static bool initialized;
};



