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

int DirectorySystem::init() {
    FAT::init();

    std::cout << "\n======Loading Tree======\n";
    root = DirectorySystem::loadTree(ROOT_BLK);
    if(!root)return -1;
    std::cout << "\n======Loading Tree FINISHED======\n";

    initialized = true;
    return 0;
}

Inode *DirectorySystem::loadTree(adisk_t block) {
    block_t fcb = {0};
    HDisk::get().readBlock(fcb, block);
    FileControlBlock::printFCB(fcb);
    Inode *node = Inode::makeNode(fcb);
    if(!node)return nullptr;

    if(node->fcb->bro)
        node->bro = loadTree(node->fcb->bro);

    if(node->fcb->child)
        node->child = loadTree(node->fcb->child);

    return node;
}

// TODO:
FHANDLE DirectorySystem::open(pathname_t path, FILE_EXT extension, size_t size) {
    if(size == 0 || size > BLOCK_SZ * FAT_SZ)return -1;
    if(!strcmp(path, "/"))return -2; // can't do root
    if(path[0] != '/')return -3; // isn't full path
    if(path[strlen(path) - 1] == '/')return -4; // no name


    if(!initialized)
        if(DirectorySystem::init() < 0) return -5;

    // TODO URGENT: first check if file exists

    block_cnt_t data_size = (size + BLOCK_SZ - 1) / BLOCK_SZ;
    fat_entry_t data_block = 0; // it's block where the data is stored
    fat_entry_t fcb_block = 0;
    FileControlBlock::fcb_t buf; // make buffer without allocating data + fcb blocks

    FileControlBlock::populateFCB(buf, path, extension, data_size, data_block, fcb_block);
    std::cout << "\nOPENING FILE\n";
    FileControlBlock::printFCB(buf);
    std::cout << "\nLINKING\n";

    Inode *prev, *node = Inode::makeNode(buf);
    int link_with_parent = Inode::findToLink(root, node, &prev);
    // try to find node to link with and then if the file doesn't exist (can be linked) return

    std::cout << " \n======Linking with ";

    switch(link_with_parent) {
        case 0: // needs linking
        case 1:
            std::cout << (link_with_parent ? "parent======\n" : "brother======\n");

            fcb_block = FAT::takeBlocks(1); // todo 1/8th
            if(!fcb_block)// no space for fcb
                return -6;

            data_block = FAT::takeBlocks(data_size);
            if(!data_block) {// no space for data
                FAT::releaseBlocks(fcb_block, 1);
                return -7;
            }
            node->fcb->data_block = data_block;
            node->fcb->fcb_block = fcb_block;

            Inode::link(link_with_parent, node, prev); // can return <0 but shouldn't (previous checks)

            memcpy(buf, node->fcb, sizeof(FileControlBlock::FCB));

            DirectorySystem::linkAndWriteFCBs(link_with_parent, buf, fcb_block, prev);
            break;
        case 2: // already exists
            std::cout << "no one======\n";
            std::cout << "ALREADY EXISTS.\n";

            data_block = prev->fcb->data_block;
            fcb_block = prev->fcb->fcb_block;
            data_size = prev->fcb->data_size;
            break;
        default: // error
            return link_with_parent;
    }

    // usually we would need FCB address to write in OFT but since it's write back, we will directly write in data block
    return oft.set(data_block,
                   0); // TODO change oft, we need only entry_index(block to start writing/reading) and cursor
}

void DirectorySystem::linkAndWriteFCBs(bool parent, FileControlBlock::fcb_t buf, fat_entry_t fcb_block, Inode *prev) {

    block_t blk = {0};

    // write fcb to disk TODO allocate 1/8th not whole block
    HDisk::get().writeBlock(buf, fcb_block);
    // linking
    if(!parent) {
        prev->fcb->bro = fcb_block;
    } else {
        prev->fcb->child = fcb_block;
    }
    // change parents or brothers fcb TODO change only 1/8th not whole block
//    FileControlBlock::populateFCB(blk, prev->fcb->path, prev->fcb->ext, prev->fcb->data_size,
//                                  prev->fcb->entry, prev->fcb->child, prev->fcb->bro,
//                                  prev->fcb->child_offs, prev->fcb->bro_offs);

    FileControlBlock::populateFCB(blk, prev->fcb); // update previous fcb
    HDisk::get().writeBlock(blk, prev->fcb->fcb_block); // write update to disk

    HDisk::get().readBlock(blk, prev->fcb->fcb_block);
    PrintHex::printBlock(blk, BLOCK_SZ, 16);
}

int DirectorySystem::close(FHANDLE file) {
    if(!initialized)return -1;
    std::cout << "\n=====CLOSING=====";
    std::cout << "\n=====FREE OFT=====\n";
    oft.releaseEntry(file);

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
    FileControlBlock::populateFCB(fcb, "/", DIR, 69, 0xff, ROOT_BLK, 0, 0, 0);
    memcpy(block, fcb, sizeof(FileControlBlock::FCB));
    HDisk::get().writeBlock(block, ROOT_BLK);
}

// level order traversal
void DirectorySystem::printTree() {
}






