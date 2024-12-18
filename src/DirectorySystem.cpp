#include "../h/DirectorySystem.h"
#include "../h/PrintHex.h"
#include "../h/HDisk.h"


Inode *DirectorySystem::root = nullptr;
bool DirectorySystem::initialized = false;
OpenFilesTable DirectorySystem::oft;

DirectorySystem::~DirectorySystem() { // TODO: Deallocate structures

    delete root;
}

void DirectorySystem::init() { // TODO: Allocate the structure
    FAT::init();

    FileControlBlock::fcb_t fcb = {0};
    block_t block = {0};

    HDisk::get().readBlock(block, ROOT_BLK);
    PrintHex::printBlock(block, BLOCK_SZ, 16);

    memcpy(fcb, block, sizeof(fcb));
    root = Inode::make_node(block);

    initialized = true;
}

// TODO:
FHANDLE DirectorySystem::open(pathname_t path, FILE_EXT extension, size_t size) {
    if(size == 0 || size > BLOCK_SZ * FAT_SZ)return -1;
    if(!initialized)init();


    block_cnt_t data_size = (size + BLOCK_SZ - 1) / BLOCK_SZ;
    fat_entry_t entry_index = FAT::take_blocks(data_size);
    if(entry_index == 0)return -2;


    FileControlBlock::fcb_t buf;
    FileControlBlock::populateFCB(buf, path, extension, data_size, entry_index);
    std::cout << "OPENING FILE\n";
    FileControlBlock::printFCB(buf);
    std::cout << "LINKING\n";
    Inode *prev;

    std::cout << std::dec << Inode::add(root, Inode::make_node(buf), &prev);

    return oft.set(entry_index, 0);
}

int DirectorySystem::close(FHANDLE file) {
    if(!initialized)return -1;
    std::cout << "\n=====CLOSING=====";
    std::cout << "\n=====FREE OFT=====\n";
    oft.release_entry(file);

    FAT::saveToDisk();

    oft.printFHANDLE(file);

//    std::cout << "\n=====CLOSED=====\n FHANDLE: " << file << "\n FAT: \n";
//
//    PrintHex::printBlock(table, 256, 16);
//    std::cout << "\n================\n";
    return 0;
}

void DirectorySystem::clearRoot() { // TODO: not tested
    block_t block = {0};
    FileControlBlock::fcb_t fcb;
    FileControlBlock::populateFCB(fcb, "/", DIR, 69, 0, 0, 0, 0, 0);
    memcpy(block, fcb, sizeof(fcb));
    HDisk::get().writeBlock(block, ROOT_BLK);
}



