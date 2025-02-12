#include "../h/OpenFilesTable.h"
#include "../h/PrintHex.h"

OpenFilesTable::OpenFilesTable() {
    for(dchar_t i = 0; i < OFT_SZ; i++) {
        table[i][0] = (dchar_t) 0;
        table[i][1] = (dchar_t) 0;
    }
}

void OpenFilesTable::printFHANDLE(FHANDLE file) {
    std::cout << "====================\n";
    std::cout << std::dec << "For FHANDLE: " << file << std::endl;
    PrintHex::print((table[file][0]), "INODE address: ");
    PrintHex::print((table[file][1]), "Cursor: ");
    std::cout << "\n====================\n";
}

void OpenFilesTable::printOFT(aoft_t limit) {
    std::cout << "==========OFT(Block 8B, Cursor 8B (should be 2B))==========\n";
    for(dchar_t i = 0; i <= limit; i++) {
        if(i > 0 && i % 8 == 0)
            std::cout << std::endl;
        PrintHex::print(table[i][0]);
        PrintHex::print(table[i][1]);
        std::cout << "|";
    }
    if(limit != OFT_SZ - 1)
        std::cout << "...";
    std::cout << "\n=======================================================\n";
}

int OpenFilesTable::set(uint64_t inode_address, uint64_t cursor) {
    int ent = takeEntry();
    std::cout << "OFT entry taken: " << std::dec << ent << std::endl;
    if(ent < 0)return -1;
    aoft_t entry = ent;
    table[entry][0] = inode_address;
    table[entry][1] = cursor;

    //OpenFilesTable::printOFT(31);
    return entry;
}

int OpenFilesTable::takeEntry() {
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

void OpenFilesTable::releaseEntry(FHANDLE fhandle) {
    if(fhandle < 0)return;
    auto i = fhandle / 64;
    auto j = fhandle % 64;
    table[fhandle][0] = 0;
    table[fhandle][1] = 0; // cursor
    free_vector[i] &= ~(uint64_t) (1 << j);
}

bool OpenFilesTable::isTaken(FHANDLE fhandle) const {
    if(fhandle < 0)return false;
    auto i = fhandle / 64;
    auto j = fhandle % 64;


    return free_vector[i] & ((uint64_t) (1 << j));
}

uint64_t OpenFilesTable::getInodeAddress(FHANDLE fhandle) const {
    return table[fhandle][0];
}

int OpenFilesTable::moveCursor(FHANDLE fhandle, int32_t val) {
    if(!isTaken(fhandle))return ERROR_FILE_NOT_OPENED;
    table[fhandle][1] += val;
    return 0;
}

int OpenFilesTable::setCursor(FHANDLE fhandle, int32_t val) {
    if(!isTaken(fhandle))return ERROR_FILE_NOT_OPENED;
    table[fhandle][1] = val;
    return OK;
}

uint64_t OpenFilesTable::getCursor(FHANDLE fhandle) const {
    if(!isTaken(fhandle))return ERROR_FILE_NOT_OPENED;
    return table[fhandle][1];
}










