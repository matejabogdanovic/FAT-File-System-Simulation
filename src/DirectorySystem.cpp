#include "../h/DirectorySystem.h"
#include "../h/PrintHex.h"
#include "../h/HDisk.h"

DirectorySystem::Directory *DirectorySystem::root = nullptr;
bool DirectorySystem::initilized = false;

DirectorySystem::~DirectorySystem() { // TODO:
    delete root;
}

void DirectorySystem::init() { // TODO:
    FAT::init();
    //root = new Directory(new FCB("root"));

    //new FCB("root", DIR, 69, 0, 0, 0);
    // fcb_t fcb;
    //populateFCB(fcb, "root", DIR, 69, 0, 0, 0);
    block_t block = {0};
    // memcpy(block, fcb, sizeof(fcb));


    HDisk::get().readBlock(block, ROOT_BLK);
    PrintHex::printBlock(block, BLOCK_SZ, 16);

    //new FCB{block};

    initilized = true;
}

FHANDLE DirectorySystem::open(filename_t name, FILE_EXT extension, size_t size) {
    if(!initilized)init();
    return FAT::open(name, extension, size);
}

int DirectorySystem::close(FHANDLE file) {
    if(!initilized)return -1;
    return FAT::close(file);
}

DirectorySystem::Directory::Directory(FileControlBlock::FCB *fcb, DirectorySystem::Directory *left,
                                      DirectorySystem::Directory *right)
        : fcb(fcb), left(left), right(right) {
}

DirectorySystem::Directory::~Directory() {
    delete fcb;
}
