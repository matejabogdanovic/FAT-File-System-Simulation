#include "../h/DirectorySystem.h"
#include "../h/PrintHex.h"
#include "../h/HDisk.h"
#include <cstring>

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
    if(!strcmp(path, "/"))return -2; // can't do root
    if(path[0] != '/')return -3; // isn't full path
    if(path[strlen(path) - 1] == '/')return -4; // no name

    if(!initialized)init();


    block_cnt_t data_size = (size + BLOCK_SZ - 1) / BLOCK_SZ;
    fat_entry_t entry_index = FAT::take_blocks(data_size); // it's block where the data is stored
    if(entry_index == 0)return -5;


    FileControlBlock::fcb_t buf;
    FileControlBlock::populateFCB(buf, path, extension, data_size, entry_index);
    std::cout << "\nOPENING FILE\n";
    FileControlBlock::printFCB(buf);
    std::cout << "\nLINKING\n";

    Inode *prev;
    int link_with_parent = Inode::add(root, Inode::make_node(buf), &prev);
    std::cout << " \n---\n" << link_with_parent;

    fat_entry_t fcb_block;
    block_t blk = {0};
    switch(link_with_parent) {
        case 0:
        case 1:
            std::cout << "\n======Linking with parent or brother======" << link_with_parent << std::endl;
            fcb_block = FAT::take_blocks(1);
            // write fcb to disk TODO allocate 1/8th not whole block
            HDisk::get().writeBlock(buf, fcb_block);
            if(link_with_parent == false) {
                prev->fcb->bro = fcb_block;
            } else {
                prev->fcb->child = fcb_block;
            }
            // change parents or brothers fcb TODO change only 1/8th not whole block
            FileControlBlock::populateFCB(blk, prev->fcb->path, prev->fcb->ext, prev->fcb->data_size,
                                          prev->fcb->entry, prev->fcb->child, prev->fcb->bro,
                                          prev->fcb->child_offs, prev->fcb->bro_offs);

            HDisk::get().writeBlock(blk, prev->fcb->entry);

            HDisk::get().readBlock(blk, prev->fcb->entry);
            PrintHex::printBlock(blk, BLOCK_SZ, 16);
            break;
        case 2:
            PrintHex::printBlock(FAT::table, BLOCK_SZ, 16);
            std::cout << "\nALREADY EXISTS.";
            FAT::release_blocks(entry_index, data_size); // TODO
            PrintHex::printBlock(FAT::table, BLOCK_SZ, 16);
            entry_index = prev->fcb->entry;
            data_size = prev->fcb->data_size;
            break;
        default: // error
            return link_with_parent;
    }

    // usually we would need FCB address to write in OFT but since it's write back, we will directly write in data block
    return oft.set(entry_index,
                   0); // TODO change oft, we need only entry_index(block to start writing/reading) and cursor
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
    FileControlBlock::populateFCB(fcb, "/", DIR, 69, ROOT_BLK, 0, 0, 0, 0);
    memcpy(block, fcb, sizeof(fcb));
    HDisk::get().writeBlock(block, ROOT_BLK);
}

// level order traversal
void DirectorySystem::printTree() {
}



