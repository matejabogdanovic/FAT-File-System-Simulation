#pragma once
#include "FAT.h"
#include "Inode.h"

class FileSystem {
public:
    /**
     * @brief
     * @return instance of FileSystem
     */
    static FileSystem &get();

    /**
     * @brief Resets root block to contain only root folder. Clears \b just the root block.
     */
    static void clearRoot(); // temporary public

    int rename(const char *path, FILE_EXT extension, const char *name);

    /**
     * @brief Removes the file found using \p path and \p extension .
     * @param path
     * @param extension
     * @return
     */
    int remove(const char *path, FILE_EXT extension);

    /**
     * @brief Prints file system tree starting from root directory.
     */
    void printTree() const;

    /**
     * @brief
     * @return current working directory name
     */
    const char *getWorkingDirectoryName() const;

    /**
     * @brief
     * @return current working directory path
     */
    const char *getWorkingDirectoryPath() const;

    /**
     * @brief Finds node using \p path and sets it as working directory.
     * @param path
     * @return <0 if error
     */
    int setWorkingDirectory(char *path);

    /**
     * @brief Prints working directory files.
     */
    void listWorkingDirectory() const;

    ~FileSystem();

private:
    friend class File;

    friend class Console;

    FileSystem();

    /**
     * @brief Helper function for finding file name from path.
     * @param path
     * @param extension
     * @param path_len return value
     * @param file_name return value
     * @return
     */
    int getFileName(char *path, FILE_EXT extension, size_t *path_len, char *file_name);

    // if FILE EXISTS, previous, status and logical parent might not be consistent => use
    // node to access these fields
    Inode::Status searchTree(const char *path, FILE_EXT extension,
                             Inode::Status *status, Inode **previous, Inode **logical_parent,
                             Inode **node, char *filename);

    /**
     * @brief
     * @param path
     * @param extension
     * @param size
     * @return
     */
    FHANDLE open(const char *path, FILE_EXT extension, size_t size);

    /**
     * @brief Saves cached FAT structures to disk and releases OFT entry taken by \p file.
     * @param file
     * @return
     */
    int close(FHANDLE file);

    /**

     */
    int close(const char *path, FILE_EXT extension);

    int close(Inode *node);

    void removeRecursive(Inode *start);

    /**
     * @brief Allocates directory tree starting from the root directory.
     * @return <0 if error
     */
    int init();

    static Inode *loadTree(adisk_t blk, Inode *logical_parent = nullptr, Inode *previous = nullptr);

    static void deallocateTree(Inode *start, bool save_to_disk = true);

    void getWorkingDirectoryPathRecursive(Inode *node, char *path) const;

    inline Inode *getWorkingDirectory() const;

    int setWorkingDirectory(Inode *working);

    inline static block_cnt_t sizeToBlocks(size_t size);

    Inode *root = nullptr;
    Inode *working = nullptr;
    char working_path[PATHNAME_SZ + 1] = {0};
    OpenFilesTable oft;
};



