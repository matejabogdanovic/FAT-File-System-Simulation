#pragma once
#include "TypesAndConstants.h"


class FileControlBlock {
public:
    struct FCB { // 32 chars => 8 FCBs in ONE block

        char name[FILE_NAME_SZ] = {0};
        FILE_EXT ext;
        block_cnt_t data_size;
        fat_entry_t entry;
        adisk_t left, right;
        char_t padding[11] = {0};

        explicit FCB(filename_t name,
                     FILE_EXT ext,
                     block_cnt_t data_size,
                     fat_entry_t entry,
                     adisk_t left,
                     adisk_t right) : ext(ext), data_size(data_size), entry(entry), left(left), right(right) {
            bool name_finished = false;
            for(size_t i = 0; i < sizeof(FCB::name); i++) {

                if(name[i] == '\0')name_finished = true;
                this->name[i] = (name_finished ? (char) 0 : name[i]);
            }
        }
    };

    typedef char_t fcb_t[sizeof(FCB)];

private:
    friend class FAT;

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
                            fat_entry_t entry_index,
                            adisk_t left = 0,
                            adisk_t right = 0);

    /**
     * @brief Prints \p fcb using a predefined format.
     * @param fcb
    */
    static void printFCB(fcb_t fcb);
};