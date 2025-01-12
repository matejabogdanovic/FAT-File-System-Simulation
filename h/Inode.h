#pragma once
#include "FileControlBlock.h"

struct Inode {


    FileControlBlock::FCB *fcb;
    Inode *child, *bro, *parent, *previous;
    bool changed = false;
    FHANDLE handle = -1;

    bool isOpened() const;

    void open(FHANDLE fhandle);

    void close();

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

    static void unlinkInode(Inode *node);

    static void linkInodes(Inode *node, Inode *prev, bool is_prev_parent, Inode *logical_parent = nullptr);

    void printInode();

    static void printTree(const Inode *node, unsigned parent_name_size = 0, int level = 0);

    ~Inode();
};


