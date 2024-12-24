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

    enum Status : int {
        ERROR_INVALID_ARGS = -1,
        ERROR_INVALID_FCB = -2,
        ERROR_INVALID_PATH_NAME = -3,
        LINK_WITH_BROTHER = 0,
        LINK_WITH_PARENT = 1,
        FILE_EXISTS = 2
    };

    /**
     * @brief Searches for a node with a given \p path and \p extension .
     * @param start starting node
     * @param path return value, wanted file path - if relative path, will be changed to full
     * @param extension wanted file extension
     * @param is_prev_parent return value, describes relation between \p previous and \p node
     * @param previous return value, previous node in a tree
     * @param node return value, if node doesn't exist, it points to - nullptr
     * @return If < 0 - error - check status. If > 0 - either is the same as *\p is_prev_parent or it is 'FILE_EXISTS'
     * indicator.
     */
    static Inode::Status searchTree(Inode *start, char *path, FILE_EXT extension,
                                    Status *is_prev_parent, Inode **previous, Inode **node);

    static void linkInodes(Inode *node, Inode *prev, bool is_prev_parent);

    void printInode();

    static void printTree(const Inode *node, unsigned parent_name_size = 0, int level = 0);

    ~Inode();
};



