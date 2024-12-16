#include "../h/DirectorySystem.h"
#include "../h/PrintHex.h"
#include "../h/HDisk.h"

DirectorySystem::Inode *DirectorySystem::root = nullptr;
bool DirectorySystem::initialized = false;

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

FHANDLE DirectorySystem::open(filename_t name, FILE_EXT extension, size_t size) {
    if(!initialized)init();
    return FAT::open(name, extension, size);
}

int DirectorySystem::close(FHANDLE file) {
    if(!initialized)return -1;
    return FAT::close(file);
}

void DirectorySystem::clearRoot() {
    FileControlBlock::fcb_t fcb;
    FileControlBlock::populateFCB(fcb, "root", DIR, 69, 0, 0, 0);
    HDisk::get().writeBlock(fcb, ROOT_BLK);
}

DirectorySystem::Inode::Inode(FileControlBlock::FCB *fcb, DirectorySystem::Inode *left,
                              DirectorySystem::Inode *right)
        : fcb(fcb), left(left), right(right) {
}

DirectorySystem::Inode::~Inode() {
    delete fcb;
}

DirectorySystem::Inode *DirectorySystem::Inode::make_node(FileControlBlock::fcb_t fcb) {

    auto *f = (FileControlBlock::FCB *) fcb;
    auto *dir = new Inode(
            new FileControlBlock::FCB(
                    *f
            )
    );

    return dir;
}
