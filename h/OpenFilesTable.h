#pragma once
#include "TypesAndConstants.h"

class OpenFilesTable {
    friend class FileSystem;

    friend class Console;

    OpenFilesTable();

    /**
     * @brief Print up to \p limit row.
     * @param limit
     */
    void printOFT(aoft_t limit = 255);

    /**
     * @brief Prints \p file using a predefined format.
     * @param file
    */
    void printFHANDLE(FHANDLE file);

    /**
     * @brief Occupies one free entry in the table.
     * @param data_block
     * @param offset_in_block
     * @return index of occupied entry
    */
    int set(adisk_t data_block, char_t offset_in_block);

    /**
     * @brief
     * @return negative if no free entries
     */
    int takeEntry();

    /**
     * @brief Frees \p fhandle entry.
     * @param fhandle
     */
    void releaseEntry(FHANDLE fhandle);

    bool isTaken(FHANDLE fhandle) const;

    /**
     * @brief
     * @param fhandle
     * @return 0 if error
     */
    adisk_t getDataBlock(FHANDLE fhandle) const;

    oft_t table{ };
    uint64_t free_vector[FREE_V_SZ] = {0};
};






