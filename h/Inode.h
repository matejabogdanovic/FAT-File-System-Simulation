#pragma once
#include "FileControlBlock.h"

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



