#pragma once
#include "FileControlBlock.h"

struct Inode {


    FileControlBlock::FCB *fcb;
    Inode *child, *bro, *parent;

    explicit Inode(FileControlBlock::FCB *fcb, Inode *child = nullptr, Inode *bro = nullptr, Inode *parent = nullptr);

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
        EXCEPTION_CAN_T_OPEN = -4,
        LINK_WITH_BROTHER = 0,
        LINK_WITH_PARENT = 1,
        FILE_EXISTS = 2
    };

    /**
     * @brief Searches for a node with a given \p path and \p extension .
     * @param start starting node
     * @param path wanted file path - if relative path, will be changed to full
     * @param extension wanted file extension
     * @param is_prev_parent return value, describes relation between \p previous and \p node
     * @param previous return value, previous node in a tree
     * @param logical_parent return value, logical parent in tree hierarchy
     * @param node return value, if node doesn't exist, it points to - nullptr
     * @return If < 0 - error - check status. If > 0 - either is the same as *\p is_prev_parent or it is 'FILE_EXISTS'
     * indicator.
     */
    static Inode::Status searchTree(Inode *start, char *path, FILE_EXT extension,
                                    Status *is_prev_parent, Inode **previous, Inode **logical_parent, Inode **node);

    static void linkInodes(Inode *node, Inode *prev, bool is_prev_parent, Inode *logical_parent = nullptr);

    void printInode();

    static void printTree(const Inode *node, unsigned parent_name_size = 0, int level = 0);

    ~Inode();

private:
    /**
     * @brief Initializes these variables just using \p path and \p root_path . Tokenization will start, then
     * just call 'strtok(NULL, "/")' to get new \p next_folder to look for.
     * @param starting_dir return value, calculated full path to the next folder/file to look for
     * @param needed_inode_name return value, calculated name of the next folder/file to look for
     * @param path_copy address used for tokenization, shouldn't be accessed!
     * @param path relative or absolute/full path to wanted file that is being opened. It will be changed to full path.
     * @param root_path absolute path to root directory or working, depending on if \p path is absolute or relative
     */
    static void openStartingDirectory(char starting_dir[FILENAME_SZ + 1],
                                      char *path,
                                      const char *root_path);
};


