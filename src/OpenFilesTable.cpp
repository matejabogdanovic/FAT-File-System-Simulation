#include "../h/OpenFilesTable.h"
#include "../h/PrintHex.h"

void OpenFilesTable::printFHANDLE(FHANDLE file) {
    std::cout << "\n====================";
    std::cout << std::dec << "\nFor FHANDLE: " << file << std::endl;
    PrintHex::print((adisk_t) (table[file][0] >> OFFS_SHIFT), "FCB block: ");
    std::cout << std::endl;
    PrintHex::print(table[file][0] & OFFS_MASK, "Offset in block: ");
    std::cout << "\n====================\n";
}

void OpenFilesTable::printOFT(aoft_t limit) {
    std::cout << "\n==========OFT(Block 1B, Offset 1B, Cursor 2B)==========\n";
    for(dchar_t i = 0; i <= limit; i++) {
        if(i > 0 && i % 8 == 0)
            std::cout << std::endl;
        PrintHex::print((adisk_t) (table[i][0] >> OFFS_SHIFT));
        PrintHex::print(table[i][0] & OFFS_MASK);
        PrintHex::print(table[i][1]);
    }
    if(limit != OFT_SZ - 1)
        std::cout << "...";
    std::cout << "\n=======================================================\n";
}

// TODO: Finish
int OpenFilesTable::set(adisk_t fcb_block, block_cnt_t offset_in_block) {
    int ent = take_entry();
    std::cout << "OFT entry taken: " << std::dec << ent;
    if(ent < 0)return -1;
    aoft_t entry = ent;
    table[entry][0] = ((dchar_t) fcb_block) << OFFS_SHIFT
                      | offset_in_block;
    table[entry][1] = 0; // cursor

    OpenFilesTable::printOFT(31);
    return entry;
}

int OpenFilesTable::take_entry() {
    int entry = -1;
    for(int i = 0; i < FREE_V_SZ; ++i) {
        if(free_vector[i] == UINT64_MAX)continue;
        // there is free entry
        uint64_t mask = 0x1;
        int j = 0;
        for(; j < 64; ++j) {
            if((free_vector[i] & mask) == 0) {
                // found
                free_vector[i] |= mask;
                break;
            }

            mask <<= 1;
        }
        entry = 64 * i + j;
        break;
    }

    return entry;
}

void OpenFilesTable::release_entry(FHANDLE fhandle) {
    auto i = fhandle / 64;
    auto j = fhandle % 64;

    free_vector[i] &= ~(uint64_t) (1 << j);
}


