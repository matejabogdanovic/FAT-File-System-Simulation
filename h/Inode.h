#pragma once
#include "FileControlBlock.h"

struct Inode {


    FileControlBlock::FCB *fcb;
    Inode *child, *bro, *parent, *previous;
    FHANDLE handle = -1;

    explicit Inode(FileControlBlock::FCB *fcb,
                   Inode *child = nullptr,
                   Inode *bro = nullptr,
                   Inode *parent = nullptr,
                   Inode *previous = nullptr);

    /**
    * @brief Creates and allocates one Directory node from \p fcb block.
    * @param fcb
    * @return Allocated node.
    */
    static Inode *makeNode(FileControlBlock::fcb_t fcb);

    enum Status : int {
        ERROR_INVALID_ARGS = -1,
        ERROR_INVALID_FCB = -2,
        ERROR_INVALID_PATH_NAME = -3,
        // EXCEPTION_CAN_T_OPEN = -4,
        LINK_WITH_BROTHER = 0,
        LINK_WITH_PARENT = 1,
        FILE_EXISTS = 2
    };

    static void linkInodes(Inode *node, Inode *prev, bool is_prev_parent, Inode *logical_parent = nullptr);

    void printInode();

    static void printTree(const Inode *node, unsigned parent_name_size = 0, int level = 0);

    ~Inode();
};


