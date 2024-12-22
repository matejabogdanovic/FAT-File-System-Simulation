#include "../h/FileControlBlock.h"
#include "../h/PrintHex.h"
#include <cstring>

void FileControlBlock::printFCB(fcb_t buf) {

    std::cout << "\n====================";
    std::cout << "\nFile name: ";
    for(size_t i = 0; i < sizeof(FCB::path); i++) {
        if(buf[i] != '\0')std::cout << buf[i];
    }

    std::cout << "\nFCB block:\n";
    PrintHex::printBlock(buf, sizeof(fcb_t), 8);
    std::cout << "\n====================\n";
}

int FileControlBlock::populateFCB(fcb_t buf,
                                  pathname_t path,
                                  FILE_EXT extension,
                                  block_cnt_t data_size,
                                  fat_entry_t data_block,
                                  adisk_t fcb_block,
                                  adisk_t child,
                                  adisk_t bro,
                                  char_t child_offs,
                                  char_t bro_offs) {

    auto *fcb = new FCB(path, extension, data_size, data_block, fcb_block, child, bro, child_offs,
                        bro_offs);

    memcpy(buf, fcb, sizeof(FileControlBlock::FCB));

    delete fcb;

    return 0;
}

int FileControlBlock::populateFCB(fcb_t buf, FileControlBlock::FCB *fcb) {

    if(fcb == nullptr || buf == nullptr)return -1;

    memcpy(buf, fcb, sizeof(FileControlBlock::FCB));
    return 0;
}

FileControlBlock::FCB::FCB(pathname_t path, FILE_EXT extension, block_cnt_t data_size, fat_entry_t data_block,
                           adisk_t fcb_block, adisk_t child, adisk_t bro, char_t child_offs, char_t bro_offs)
        : ext(extension), data_size(data_size),
          data_block(data_block), fcb_block(fcb_block), child(child),
          bro(bro), child_offs(child_offs), bro_offs(bro_offs) {


    strcpy(this->path, path);
}
