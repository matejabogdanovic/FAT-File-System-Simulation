#include "../h/FileControlBlock.h"
#include "../h/PrintHex.h"
#include "../h/HDisk.h"
#include <cstring>

void FileControlBlock::printFCBt(fcb_t fcb) {

    std::cout << "====================\n";
    std::cout << "File name: ";
    for(size_t i = 0; i < sizeof(FCB::name); i++) {
        if(fcb[i] != '\0')std::cout << fcb[i];
    }

    std::cout << "\nFCB block:\n";
    PrintHex::printBlock(fcb, sizeof(fcb_t), 8);
    std::cout << "\n====================";
}

int FileControlBlock::populateFCB(fcb_t buf,
                                  const char path[FILENAME_SZ + 1],
                                  FILE_EXT extension,
                                  block_cnt_t data_size,
                                  adisk_t data_block,
                                  adisk_t fcb_block,
                                  adisk_t fcb_block_offs,
                                  adisk_t child,
                                  char_t child_offs,
                                  adisk_t bro,
                                  char_t bro_offs) {

    auto *fcb = new FCB(path, extension, data_size, data_block, fcb_block, child, child_offs, bro,
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

void FileControlBlock::linkFCBs(FCB *fcb, FCB *prev_fcb,
                                bool is_prev_parent) {

    // write fcb to disk TODO allocate 1/8th not whole block
    if(!is_prev_parent) {
        prev_fcb->bro = fcb->fcb_block;
    } else {
        prev_fcb->child = fcb->fcb_block;
    }
    // change parents or brothers fcb TODO change only 1/8th not whole block
    /*
     block_t blk = {0};
     HDisk::get().readBlock(blk, prev_fcb->fcb_block);
     PrintHex::printBlock(blk, BLOCK_SZ, 16);
     std::cout << std::endl;
     */
}

// todo change to 1/8th
void FileControlBlock::saveFCBtoDisk(FileControlBlock::FCB *fcb) {
    fcb_t buf = {0};
    FileControlBlock::populateFCB(buf, fcb);
    HDisk::get().writeBlock(buf, fcb->fcb_block);
}

void FileControlBlock::unlinkFCBs(FileControlBlock::FCB *fcb, FileControlBlock::FCB *prev_fcb, bool is_prev_parent) {
    if(is_prev_parent) {
        prev_fcb->child = fcb->bro;
        prev_fcb->child_offs = fcb->bro_offs;
    } else {
        prev_fcb->bro = fcb->bro;
        prev_fcb->bro_offs = fcb->bro_offs;
    }
}

FileControlBlock::FCB::FCB(const char name[FILENAME_SZ + 1], FILE_EXT extension, block_cnt_t data_size,
                           adisk_t data_block,
                           adisk_t fcb_block,
                           adisk_t fcb_block_offs,
                           adisk_t child,
                           char_t child_offs,
                           adisk_t bro,
                           char_t bro_offs)
        : ext(extension),
          data_size(data_size),
          data_block(data_block),
          fcb_block(fcb_block),
          fcb_block_offs(fcb_block_offs),
          child(child),
          child_offs(child_offs),
          bro(bro),
          bro_offs(bro_offs) {


    strcpy(this->name, name);
}

void FileControlBlock::FCB::printFCB() {
    std::cout
            << "rwx "
            << this->name << "." << file_ext_str[ext] << " "
            << std::dec << data_size * BLOCK_SZ << "B";

//    PrintHex::print(data_size, "data_size:");
//    PrintHex::print(data_block, "data_block:");
//    PrintHex::print(fcb_block, "fcb_block:");
//    PrintHex::print(child, "child:");
//    PrintHex::print(bro, "bro:");
//    PrintHex::print(child_offs, "child_offs:");
//    PrintHex::print(bro_offs, "bro_offs:");
}
