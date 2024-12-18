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
    static Inode *make_node(FileControlBlock::fcb_t fcb);

    /**
     * @brief Links \p node into tree, starting search from \p root.
     * @param root
     * @param node
     * @param link_to node that it's linked with
     * @param parent \p link_to is parent?
     * @return <0 if error
     */
    static int add(Inode *root, Inode *node, Inode **link_to);

    ~Inode();
};



