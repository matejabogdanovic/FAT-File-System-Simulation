#pragma once
#include "TypesAndConstants.h"


class FileControlBlock {
public:
    struct FCB { // 32 chars => 8 FCBs in ONE block

        char path[PATH_NAME_SZ] = {0};
        FILE_EXT ext;
        block_cnt_t data_size;
        fat_entry_t entry;
        adisk_t child, bro;
        char_t child_offs, bro_offs;
        char_t padding[9] = {0};
        // char_t padding[11] = {0};


//        explicit FCB(filename_t name,
//                     FILE_EXT ext,
//                     block_cnt_t data_size,
//                     fat_entry_t entry,
//                     adisk_t left,
//                     adisk_t right) : ext(ext), data_size(data_size), entry(entry), left(left), right(right) {
//            bool name_finished = false;
//            for(size_t i = 0; i < sizeof(FCB::name); i++) {
//
//                if(name[i] == '\0')name_finished = true;
//                this->name[i] = (name_finished ? (char) 0 : name[i]);
//            }
//        }
    };
    typedef char_t fcb_t[sizeof(FCB)];

private:
    friend class FAT;

    friend class DirectorySystem;

    /**
     * @brief Populates \p buf with given parameters.
     * @param buf
     * @param path
     * @param extension
     * @param data_size
     * @param entry_index
     * @return <0 if error
    */
    static int populateFCB(fcb_t buf,
                           pathname_t path,
                           FILE_EXT extension,
                           block_cnt_t data_size,
                           fat_entry_t entry_index,
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