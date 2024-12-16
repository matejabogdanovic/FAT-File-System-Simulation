#include "../h/FAT.h"
#include "../h/PrintHex.h"
#include "../h/HDisk.h"
#include "../h/FileControlBlock.h"

OpenFilesTable FAT::oft;
fat_entry_t FAT::free_blocks_head = 3;
fat_entry_t FAT::free_blocks_tail = 0xff;
fat_t FAT::table;
block_t FAT::control = {0};

void FAT::init() {

    HDisk::get().readBlock(table, FAT_BLK);
    HDisk::get().readBlock(control, CONTROL_BLK);
    free_blocks_head = control[0];
}

// finds free blocks, allocates space, makes FCB for the file
// returns an entry from table of open files
FHANDLE FAT::open(filename_t name, FILE_EXT extension, size_t size) {
    if(size == 0 || size > BLOCK_SZ * FAT_SZ)return -1;

    block_cnt_t data_size = (size + BLOCK_SZ - 1) / BLOCK_SZ;
    fat_entry_t entry_index = take_blocks(data_size);
    if(entry_index == 0)return -2;


    FileControlBlock::fcb_t buf;
    FileControlBlock::populateFCB(buf, name, extension, data_size, entry_index);

    FileControlBlock::printFCB(buf);

    return oft.set(entry_index, 0); // TODO: implement offset_in_block
}

int FAT::close(FHANDLE file) {

    std::cout << "\n=====CLOSING=====";
    std::cout << "\n=====FREE OFT=====\n";
    oft.release_entry(file);
    std::cout << "\n=====FAT=>DISK[0]=====\n";
    HDisk::get().writeBlock(table, FAT_BLK);
    std::cout << "\n=====CONTROL=>DISK[1]=====\n";
    control[0] = free_blocks_head;
    HDisk::get().writeBlock(control, CONTROL_BLK);
    oft.printFHANDLE(file);
//    std::cout << "\n=====CLOSED=====\n FHANDLE: " << file << "\n FAT: \n";
//
//    PrintHex::printBlock(table, 256, 16);
//    std::cout << "\n================\n";
    return 0;
}

// returns index in FAT of the first free block
// invalid returns 0
fat_entry_t FAT::take_blocks(block_cnt_t num) {
    if(free_blocks_head == 0)return 0;
    //if (!initilized)init();


    fat_entry_t start = free_blocks_head, last = free_blocks_head;

    for(uint16_t i = 0; i < num; i++) {
        if(free_blocks_head == 0) {
            free_blocks_head = start; // there is not enough space to allocate, so reset
            break;
        }
        last = free_blocks_head;
        free_blocks_head = table[free_blocks_head];
    }
    if(free_blocks_head == start)return 0;

    table[last] = 0;

    return start;
}

void FAT::release_blocks(adisk_t start, block_cnt_t num) {
}

void FAT::clearFAT() {
    uint8_t buf[BLOCK_SZ];
    control[0] = 3;

    buf[FAT_BLK] = 0; // fat block
    buf[CONTROL_BLK] = 0; // for storing fat data on disk
    buf[ROOT_BLK] = 0; // root directory

    for(uint16_t i = 3; i < BLOCK_SZ; i++) {
        buf[i] = (uint8_t) i + 1;
    }

    HDisk::get().writeBlock(buf, FAT_BLK);
    HDisk::get().writeBlock(control, CONTROL_BLK);
}

void FAT::clearMemory() {
    block_t buf;
    for(uint16_t i = 0; i < BLOCK_SZ; i++) {
        buf[i] = (uint8_t) 0;
    }
    for(uint16_t i = 0; i < BLOCK_SZ; i++) {
        HDisk::get().writeBlock(buf, i);
    }
}





