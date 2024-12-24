#include "../h/FileSystem.h"
#include "../h/PrintHex.h"
#include "../h/HDisk.h"
#include <cstring>


FileSystem::~FileSystem() { // TODO: Deallocate structures
    delete root;
}

int FileSystem::init() {

    std::cout << "\n======Loading Tree======\n";
    root = FileSystem::loadTree(ROOT_BLK);
    if(!root)return -1;
    std::cout << "\n======Loading Tree FINISHED======\n";
    working = root;
    //root->printInode();

    initialized = true;
    return 0;
}

// todo should be done with block + offset
Inode *FileSystem::loadTree(adisk_t block) {
    block_t fcb = {0};
    HDisk::get().readBlock(fcb, block);
    FileControlBlock::printFCBt(fcb);
    Inode *node = Inode::makeNode(fcb);
    if(!node)return nullptr;

    if(node->fcb->bro)
        node->bro = loadTree(node->fcb->bro);

    if(node->fcb->child)
        node->child = loadTree(node->fcb->child);

    return node;
}

FHANDLE FileSystem::open(pathname_t path, FILE_EXT extension, size_t size) {

    block_cnt_t data_size = sizeToBlocks(size);

    // trying to find node to link current file
    FileControlBlock::FCB *fcb = new FileControlBlock::FCB(path, extension, data_size, 0, 0);
    Inode *prev, *node = new Inode(fcb);
    int is_prev_parent = Inode::findToLink(root, node, &prev);

    std::cout << " \n======Linking with ";

    adisk_t data_block = 0; // it's block where the data is stored
    adisk_t fcb_block = 0;
    switch(is_prev_parent) {
        case 0: // needs linking
        case 1:
            std::cout << (is_prev_parent ? "parent======\n" : "brother======\n");

            if(FAT::allocateFileSpace(&fcb_block, &data_block, data_size) < 0) {
                delete node;
                return -1;
            }
            // update fcb
            node->fcb->data_block = data_block;
            node->fcb->fcb_block = fcb_block;

            // link FCBs on disk
            FileControlBlock::linkFCBs(node->fcb, prev->fcb, is_prev_parent);

            // link tree nodes
            Inode::linkInodes(node, prev, is_prev_parent);
            break;
        case 2: // already exists
            std::cout << "no one======\n";
            std::cout << "ALREADY EXISTS.\n";
            // deallocate unnecessary node
            delete node;
            node = prev;
            fcb = node->fcb;
            break;
        default: // error
            return is_prev_parent;
    }

    // usually we would need FCB address to write in OFT but since it's write back, we will directly write in data block
    return oft.set(fcb->data_block,
                   0); // TODO change oft, we need only entry_index(block to start writing/reading) and cursor
}

int FileSystem::close(FHANDLE file) {
    std::cout << "\n=====CLOSING: " << file << "=====";
    std::cout << "\n=====FREE OFT=====\n";
    oft.releaseEntry(file);
    std::cout << "OFT entry freed.\n";
    FAT::saveToDisk();

    oft.printFHANDLE(file);
    std::cout << "=====CLOSED: " << file << "=====\n";
    return 0;
}

void FileSystem::clearRoot() {
    block_t block = {0};
    FileControlBlock::fcb_t fcb = {0};
    FileControlBlock::populateFCB(fcb, "/", DIR, 0, 0, ROOT_BLK, 0, 0, 0, 0);
    memcpy(block, fcb, sizeof(FileControlBlock::FCB));
    HDisk::get().writeBlock(block, ROOT_BLK);
}

int FileSystem::printTree() {
    Inode::printTree(FileSystem::get().root);
    return 0;
}

FileSystem &FileSystem::get() {
    static FileSystem dir;
    return dir;
}

FileSystem::FileSystem() {
    FAT::init();

    FileSystem::init();
}

Inode *FileSystem::getWorkingDirectory() const {
    return working;
}

int FileSystem::setWorkingDirectory(Inode *dir) {
    if(!working) return -1;
    FileSystem::working = dir;
    return 0;
}

const char *FileSystem::getWorkingDirectoryName() {
    return working->fcb->path;
}

block_cnt_t FileSystem::sizeToBlocks(size_t size) {
    return (size + BLOCK_SZ - 1) / BLOCK_SZ;
}






