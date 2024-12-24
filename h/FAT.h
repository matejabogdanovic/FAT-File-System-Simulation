#pragma once
#include "OpenFilesTable.h"

class FAT {
public: // temporary
    /**
     * @brief Resets FAT to 0 and occupies needed blocks.
    */
    static void clearFAT();

    /**
     * @brief Resets memory to 0.
    */
    static void clearMemory();

private:
    friend class FileSystem;

    /**
     * @brief Reads FAT and Control blocks from disk and initializes class fields.
    */
    static void init();

    static fat_entry_t free_blocks_head;
    static fat_entry_t free_blocks_tail;
    static fat_t table;
    static block_t control; // [0] = free_blocks_head

    /**
     * @brief Writes FAT table and control block to disk.
     */
    static void saveToDisk();

    /**
     * @brief Allocates given number of blocks.
     * @param num number of blocks to allocate
     * @return 0 if no space
    */
    static fat_entry_t takeBlocks(block_cnt_t num);

    /**
     * @brief Tries to allocate blocks for fcb and data.
     * @param fcb_block return value
     * @param data_block return value
     * @param data_size data size in blocks
     * @return
     */
    static int allocateFileSpace(adisk_t *fcb_block, adisk_t *data_block, const block_cnt_t data_size);

    /**
     * @brief Frees \p num of blocks.
     * @param start first block
     * @param num number of blocks to release from \p start
     */
    static void releaseBlocks(adisk_t start, block_cnt_t num);
};

