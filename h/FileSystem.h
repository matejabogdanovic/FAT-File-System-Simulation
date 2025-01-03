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

    /**
     * @brief Changes the name of a file or directory.
     * @param path relative or absolute path to the file or directory
     * @param name new file name
     * @return <0 if error
     */
    int rename(const char *path, const char *name);

    /**
     * @brief Deletes a file or directory.
     * @param path relative or absolute path to the file or directory
     * @return <0 if error
     */
    int remove(const char *path);

    /**
     * @brief Prints file system tree starting from the root directory.
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
     * @brief Finds a directory using and sets it as working directory.
     * @param path relative or absolute path to the directory
     * @return <0 if error
     */
    int setWorkingDirectory(char *path);

    /**
     * @brief Prints working directory files.
     */
    void listWorkingDirectory() const;

    /**
     * @brief Resets memory to 0.
    */
    static void clearMemory();

    /**
     * @brief Calls 'static void deallocateTree(Inode *start, bool save_to_disk = true)'.
     */
    ~FileSystem();

private:
    friend class File;

    friend class Console;

    FileSystem();

    /**
     * @brief Helper function for finding a file name and extension.
     * @param path
     * @param extension
     * @param path_len
     * @param file_name
     * @return
     */
    int getFileName(char *path, FILE_EXT *extension, size_t *path_len, char *file_name);

    // if FILE EXISTS, previous, status and logical parent might not be consistent => use
    // node to access these fields
    /**
     * @brief Using \p path  initializes other arguments.
     * @param path relative or absolute path to the wanted file or directory
     * @param extension return value - file extension
     * @param status return value - tells a relation between \p previous and \p node
     * @param previous return value - node that is behind the \p node
     * @param logical_parent return value - a parent directory of \p node
     * @param node return value - if file or directory doesn't exist - nullptr
     * @param filename return value - last part of \p path which represents a file or directory name, but it
     * doesn't have to be the real name (if a relative path is used it can be . or ..), so if the file exists, access
     * it's name using \p node
     * @return indicator of error (if <0), indicator if the file or directory exists or if the file or directory doesn't exist
     * , and if there are no errors, it's the same as \p status
     */
    Inode::Status searchTree(const char *path, FILE_EXT *extension,
                             Inode::Status *status, Inode **previous, Inode **logical_parent,
                             Inode **node, char *filename);

    /**
     * @brief If a file doesn't exist - creates it (allocates space for a FCB and file data using File Allocation Table,
     * links the file in a directory tree and takes an Open Files Table entry.
     * @param path
     * @param size
     * @return handle - entry in OFT, <0 if error
     */
    FHANDLE open(const char *path, size_t size);

    /**
     * @brief Saves cached FAT structures to disk and releases OFT entry taken by \p file.
     * @param file
     * @return <0 if error
     */
    int close(FHANDLE file);

    /**
     * @brief Finds the file using \p path and calls 'int close(Inode *node)'.
     * @param path
     * @return <0 if error
     */
    int close(const char *path);

    /**
     * @brief Finds the file handle using \p node and calls 'int close(FHANDLE file)'.
     * @param path
     * @return <0 if error
     */
    int close(Inode *node);

    /**
     * @brief Removes subdirectories including \p start. Frees taken space for FCB and file data, then,
     * deletes it from the directory tree.
     * @param start
     */
    void removeRecursive(Inode *start);

    /**
     * @brief Allocates directory tree starting from the root directory. Sets working directory.
     * @return <0 if error
     */
    int init();

    /**
     * @brief Creates directory tree structure recursively.
     * @param blk
     * @param logical_parent
     * @param previous
     * @return root node or nullptr
     */
    static Inode *loadTree(adisk_t blk, Inode *logical_parent = nullptr, Inode *previous = nullptr);

    /**
     * @brief Deallocates directory tree structure recursively.
     * @param start
     * @param save_to_disk if true, saves dirty (changed) FCBs to disk.
     */
    static void deallocateTree(Inode *start, bool save_to_disk = true);

    /**
     * @brief Starting from \p node, goes up to root node and creates an absolute path.
     * @param node starting node
     * @param path argument and return value
     */
    void getWorkingDirectoryPathRecursive(Inode *node, char *path) const;

    /**
     * @brief
     * @return working directory node
     */
    inline Inode *getWorkingDirectory() const;

    /**
     * @brief Changes a working directory to \p working and updates working_path.
     * @param working
     * @return <0 if error
     */
    int setWorkingDirectory(Inode *working);

    /**
     * @brief Converts data size from bytes to blocks.
     * @param size data size in bytes
     * @return converted size
     */
    inline static block_cnt_t sizeToBlocks(size_t size);

    Inode *root = nullptr;
    Inode *working = nullptr;
    char working_path[PATHNAME_SZ + 1] = {0};
    OpenFilesTable oft;
};



