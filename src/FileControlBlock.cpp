#include "../h/FileControlBlock.h"
#include "../h/PrintHex.h"

void FileControlBlock::printFCB(fcb_t buf) {

    std::cout << "\n====================";
    std::cout << "\nFile name: ";
    for(size_t i = 0; i < sizeof(FCB::name); i++) {
        if(buf[i] != '\0')std::cout << buf[i];
    }

    std::cout << "\nFCB block:\n";
    PrintHex::printBlock(buf, sizeof(fcb_t), 8);
    std::cout << "\n====================\n";
}

// TODO: Do with memcpy
void FileControlBlock::populateFCB(fcb_t buf,
                                   filename_t name,
                                   FILE_EXT extension,
                                   block_cnt_t data_size,
                                   fat_entry_t entry_index,
                                   adisk_t child,
                                   adisk_t bro,
                                   char_t child_offs,
                                   char_t bro_offs) {
    bool name_finished = false;
    size_t i = 0;
    for(size_t j = 0; j < sizeof(FCB::name); j++, i++) {

        if(name[i] == '\0')name_finished = true;
        buf[i] = (name_finished ? 0 : name[i]);
    }
    for(size_t j = 0; j < sizeof(FCB::ext); j++, i++) {
        buf[i] = extension;
    }
    for(size_t j = 0; j < sizeof(FCB::data_size); j++, i++) {
        buf[i] = data_size;
    }
    for(size_t j = 0; j < sizeof(FCB::entry); j++, i++) {
        buf[i] = entry_index;
    }
    for(size_t j = 0; j < sizeof(FCB::child); j++, i++) {
        buf[i] = child;
    }
    for(size_t j = 0; j < sizeof(FCB::bro); j++, i++) {
        buf[i] = bro;
    }
    for(size_t j = 0; j < sizeof(FCB::child_offs); j++, i++) {
        buf[i] = child_offs;
    }
    for(size_t j = 0; j < sizeof(FCB::bro_offs); j++, i++) {
        buf[i] = bro_offs;
    }
    for(size_t j = 0; j < sizeof(FCB::padding); j++, i++) {
        buf[i] = 0;
    }
}



