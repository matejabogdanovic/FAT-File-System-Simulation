#pragma once
#include "FileControlBlock.h"

struct Inode {


    FileControlBlock::FCB *fcb;
    Inode *child, *bro;

    explicit Inode(FileControlBlock::FCB *fcb, Inode *child = nullptr, Inode *bro = nullptr);

    /**
    * @brief Creates and allocates one Directory node from \p fcb block.
    * @param fcb
    * @return Allocated node.
    */
    static Inode *makeNode(FileControlBlock::fcb_t fcb);

    /**
     * @brief Finds parent/brother to link \p node into tree, starting search from \p root.
     * @return <0 if error
     */
    static int findToLink(Inode *root, Inode *node, Inode **link_to);

    static int link(bool link_with_parent, Inode *node, Inode *prev);

    ~Inode();
};



