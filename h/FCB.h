#pragma once
#include "TypesAndConstants.h"
#include "PrintHex.h"
#include <iostream>

/**
 * @brief File control block. Purely to have all FCB field sizes in one place.
 */
struct FCB { // 32 chars => 8 FCBs in ONE block
    filename_t name;
    FILE_EXT ext;
    block_cnt_t data_size;
    fat_entry_t entry;
    adisk_t left, right;

    char_t padding[11];

};

typedef char_t fcb_t[sizeof(FCB)];


/**
 * @brief Populates \p buf with given parameters.
 * @param buf
 * @param name
 * @param extension
 * @param data_size
 * @param entry_index
*/
static void populateFCB(fcb_t buf,
                        filename_t name,
                        FILE_EXT extension,
                        block_cnt_t data_size,
                        fat_entry_t entry_index);
/**
 * @brief Prints \p fcb using a predefined format.
 * @param fcb
*/
static void printFCB(fcb_t fcb);