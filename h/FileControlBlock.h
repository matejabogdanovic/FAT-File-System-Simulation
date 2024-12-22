#pragma once
#include "TypesAndConstants.h"


class FileControlBlock {
public:
    struct FCB { // 32 chars => 8 FCBs in ONE block

        char path[PATH_NAME_SZ] = {0};
        FILE_EXT ext;
        block_cnt_t data_size;
        fat_entry_t data_block;
        adisk_t fcb_block;
        adisk_t child, bro;
        char_t child_offs, bro_offs;
        char_t padding[8] = {0};
        // char_t padding[11] = {0};


        explicit FCB(pathname_t path,
                     FILE_EXT extension,
                     block_cnt_t data_size,
                     fat_entry_t data_block,
                     adisk_t fcb_block,
                     adisk_t child,
                     adisk_t bro,
                     char_t child_offs,
                     char_t bro_offs);
    };

    typedef char_t fcb_t[sizeof(FCB)];

private:

    friend class FAT;

    friend class DirectorySystem;

    /**
     * @brief Populates \p buf with given parameters.
     * @param buf should be initialized with 0
     * @return <0 if error
    */
    static int populateFCB(fcb_t buf,
                           pathname_t path,
                           FILE_EXT extension,
                           block_cnt_t data_size,
                           fat_entry_t data_block,
                           adisk_t fcb_block = 0,
                           adisk_t child = 0,
                           adisk_t bro = 0,
                           char_t child_offs = 0,
                           char_t bro_offs = 0);

    static int populateFCB(fcb_t buf, FileControlBlock::FCB *fcb);

    /**
     * @brief Prints \p fcb using a predefined format.
     * @param fcb
    */
    static void printFCB(fcb_t fcb);
};