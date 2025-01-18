#pragma once
#include "TypesAndConstants.h"
#include <cstdio>


class HDisk {
public: // shouldn't be public in real file system :)


    /**
     * @brief Reads a block from \p blck_num address and writes it in \p buf.
     * @param buf
     * @param blck_num
     * @return
    */
    int readBlock(block_t buf, adisk_t blck_num = 0);

    /**
     * @brief Writes a block from \p buf to \p blck_num address on a disk.
     * @param buf
     * @param blck_num
     * @return
    */
    int writeBlock(block_t buf, adisk_t blck_num = 0);

    /**
     * @brief
     * @return a static reference to HDisk
    */
    static HDisk &get();

    ~HDisk();

private:
    FILE *disk;

    HDisk();
};

