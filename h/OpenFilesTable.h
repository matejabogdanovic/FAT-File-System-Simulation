#pragma once
#include "TypesAndConstants.h"

class OpenFilesTable {
public:

    OpenFilesTable(){
        for(dchar_t i = 0; i < OFT_SZ; i++) {
            table[i][0] = i+1;
            table[i][1] = (dchar_t)0;

        }

    }
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
     * @param fcb_block
     * @param offset_in_block
     * @return index of occupied entry
    */
    int set(adisk_t fcb_block, char_t offset_in_block);
    /**
     * @brief
     * @return negative if no free entries
     */
    int take_entry();
    /**
     * @brief Frees \p fhandle entry.
     * @param fhandle
     */
    void release_entry(FHANDLE fhandle);
private:
    oft_t table{};
    uint64_t free_vector[FREE_V_SZ] = {0};
};






