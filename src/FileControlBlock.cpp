#include "../h/FileControlBlock.h"
#include "../h/PrintHex.h"
#include "../h/HDisk.h"
#include <cstring>

void FileControlBlock::printFCBt(fcb_t fcb) {

    std::cout << "\n====================";
    std::cout << "\nFile name: ";
    for(size_t i = 0; i < sizeof(FCB::path); i++) {
        if(fcb[i] != '\0')std::cout << fcb[i];
    }

    std::cout << "\nFCB block:\n";
    PrintHex::printBlock(fcb, sizeof(fcb_t), 8);
    std::cout << "\n====================\n";
}

int FileControlBlock::populateFCB(fcb_t buf,
                                  const char path[PATH_NAME_SZ],
                                  FILE_EXT extension,
                                  block_cnt_t data_size,
                                  adisk_t data_block,
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

void FileControlBlock::linkFCBs(const FCB *fcb, FCB *prev_fcb,
                                bool is_prev_parent) {

    block_t blk = {0};
    FileControlBlock::fcb_t buf = {0};
    memcpy(buf, fcb, sizeof(FileControlBlock::FCB));
    // write fcb to disk TODO allocate 1/8th not whole block
    HDisk::get().writeBlock(buf, fcb->fcb_block);
    // linking
    if(!is_prev_parent) {
        prev_fcb->bro = fcb->fcb_block;
    } else {
        prev_fcb->child = fcb->fcb_block;
    }
    // change parents or brothers fcb TODO change only 1/8th not whole block

    FileControlBlock::populateFCB(blk, prev_fcb); // update previous fcb
    HDisk::get().writeBlock(blk, prev_fcb->fcb_block); // write update to disk

    HDisk::get().readBlock(blk, prev_fcb->fcb_block);
    PrintHex::printBlock(blk, BLOCK_SZ, 16);
}

FileControlBlock::FCB::FCB(const char path[PATH_NAME_SZ], FILE_EXT extension, block_cnt_t data_size,
                           adisk_t data_block,
                           adisk_t fcb_block, adisk_t child, adisk_t bro, char_t child_offs, char_t bro_offs)
        : ext(extension), data_size(data_size),
          data_block(data_block), fcb_block(fcb_block), child(child),
          bro(bro), child_offs(child_offs), bro_offs(bro_offs) {


    strcpy(this->path, path);
}

void FileControlBlock::FCB::printFCB() {
    std::cout
            << "rwx "
            << this->path << "." << file_ext_str[ext] << " "
            << std::dec << data_size * BLOCK_SZ << "B";

//    PrintHex::print(data_size, "data_size:");
//    PrintHex::print(data_block, "data_block:");
//    PrintHex::print(fcb_block, "fcb_block:");
//    PrintHex::print(child, "child:");
//    PrintHex::print(bro, "bro:");
//    PrintHex::print(child_offs, "child_offs:");
//    PrintHex::print(bro_offs, "bro_offs:");
}
