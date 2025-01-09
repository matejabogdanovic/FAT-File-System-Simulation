#pragma once
#include "TypesAndConstants.h"


class FileControlBlock {
public:
    struct FCB { // 32 chars => 8 FCBs in ONE block

        char name[FILENAME_SZ + 1] = {0};
        FILE_EXT ext;
        block_cnt_t data_size;
        adisk_t data_block;
        adisk_t end_of_file_block = 0;
        char_t end_of_file_offs = 0;
        adisk_t fcb_block;
        adisk_t fcb_block_offs;
        adisk_t child;
        char_t child_offs;
        adisk_t bro;
        char_t bro_offs;
        char_t padding[5] = {0};
        // char_t padding[11] = {0};


        explicit FCB(const char name[FILENAME_SZ + 1],
                     FILE_EXT extension,
                     block_cnt_t data_size,
                     adisk_t data_block,
                     adisk_t fcb_block,
                     adisk_t fcb_block_offs = 0,
                     adisk_t child = 0,
                     char_t child_offs = 0,
                     adisk_t bro = 0,
                     char_t bro_offs = 0);

        void printFCB();
    };

    typedef char_t fcb_t[sizeof(FCB)];

    /**
     * @brief Prints \p fcb using a predefined format.
     * @param fcb
    */
    static void printFCBt(fcb_t fcb);


private:

    friend class FAT;

    friend class FileSystem;

    /**
     * @brief Populates \p buf with given parameters.
     * @param buf should be initialized with 0
     * @return <0 if error
    */
    static int populateFCB(fcb_t buf,
                           const char path[FILENAME_SZ + 1],
                           FILE_EXT extension,
                           block_cnt_t data_size,
                           adisk_t data_block,
                           adisk_t fcb_block = 0,
                           adisk_t fcb_block_offs = 0,
                           adisk_t child = 0,
                           char_t child_offs = 0,
                           adisk_t bro = 0,
                           char_t bro_offs = 0);

    static int populateFCB(fcb_t buf, FileControlBlock::FCB *fcb);

    static void linkFCBs(FCB *fcb, FCB *prev_fcb, bool is_prev_parent);

    static void unlinkFCBs(FCB *fcb, FCB *prev_fcb, bool is_prev_parent);

    static void saveFCBtoDisk(FCB *fcb);
};