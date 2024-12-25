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
    //Inode::printTree(root);
    std::cout << "\n======Loading Tree FINISHED======\n";
    working = root;

    //root->printInode();
    setWorkingDirectory(root->child->bro->child); // TODO REMOVE
    initialized = true;
    return 0;
}

// todo should be done with block + offset
Inode *FileSystem::loadTree(adisk_t block, Inode *logical_parent) {
    block_t fcb = {0};
    HDisk::get().readBlock(fcb, block);
    //FileControlBlock::printFCBt(fcb);
    Inode *node = Inode::makeNode(fcb);
    if(!node)return nullptr;
    if(logical_parent) {
        node->parent = logical_parent;
    }

    if(node->fcb->bro)
        node->bro = loadTree(node->fcb->bro, logical_parent);

    if(node->fcb->child)
        node->child = loadTree(node->fcb->child, node);

    return node;
}

FHANDLE FileSystem::open(pathname_t pathname, FILE_EXT extension, size_t size) {

    block_cnt_t data_size = sizeToBlocks(size);

    // trying to find node to link current file
    FileControlBlock::FCB *fcb = nullptr;
    Inode *prev = nullptr, *logical_parent = nullptr, *node = nullptr;
    Inode::Status is_prev_parent;
    // if it's full path, start from root, else from working dir
    Inode *start = (pathname[0] != '/' ? getWorkingDirectory() : root);
    char path[PATH_NAME_SZ] = {0}; // copy path name
    strcpy(path, pathname);

    Inode::Status ret = Inode::searchTree(start, path, extension, &is_prev_parent, &prev, &logical_parent, &node);
    if(ret < 0)return ret;
    printTree();
    std::cout << "Node: " << path << std::endl;
    std::cout << "Previous node: " << prev->fcb->path << std::endl;
    std::cout << "Previous is " << (is_prev_parent ? "parent\n" : "brother\n");
    std::cout << "Logical parent is: " << logical_parent->fcb->path << std::endl;
    if(node && node->parent)
        std::cout << "Written logical parent is: " << node->parent->fcb->path << std::endl;

    std::cout << " \n======Linking with ";

    adisk_t data_block = 0; // it's block where the data is stored
    adisk_t fcb_block = 0;

    if(ret == Inode::FILE_EXISTS) {
        std::cout << "no one======\n";
        std::cout << "ALREADY EXISTS.\n";
        fcb = node->fcb;
    } else {
        std::cout << (is_prev_parent ? "parent======\n" : "brother======\n");

        if(FAT::allocateFileSpace(&data_block, data_size, &fcb_block) < 0) {
            delete node;
            return -1;
        }
        fcb = new FileControlBlock::FCB(path, extension, data_size, data_block, fcb_block);
        node = new Inode(fcb);

        // link FCBs on disk
        FileControlBlock::linkFCBs(node->fcb, prev->fcb, is_prev_parent);

        // link tree nodes
        Inode::linkInodes(node, prev, is_prev_parent);
    }


    // usually we would need FCB address to write in OFT but since it's write back, we will directly write in data block
    return oft.set(fcb->data_block,
                   0); // TODO change oft, we need only entry_index(block to start writing/reading) and cursor
}

int FileSystem::close(FHANDLE file) {
    std::cout << "\n=====CLOSING: " << file << "=====";
    std::cout << "\n=====FREE OFT=====\n";
    oft.printFHANDLE(file);

    oft.releaseEntry(file);
    std::cout << "OFT entry freed.\n";

    FAT::saveToDisk();


    std::cout << "=====CLOSED: " << file << "=====\n";
    return 0;
}

void FileSystem::clearRoot() {
    block_t block = {0};
    FileControlBlock::fcb_t fcb = {0};
    FileControlBlock::populateFCB(fcb, "/", DIR, 0, 0, ROOT_BLK);
    memcpy(block, fcb, sizeof(FileControlBlock::FCB));
    HDisk::get().writeBlock(block, ROOT_BLK);
}

void FileSystem::printTree() const {
    Inode::printTree(root);
}

FileSystem &FileSystem::get() {
    static FileSystem dir;
    return dir;
}

FileSystem::FileSystem() {
    FAT::init();

    if(FileSystem::init() < 0)throw -1;
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

int FileSystem::remove(pathname_t pathname, FILE_EXT ext) { // todo optimize to search from working directory


    Inode *start = (pathname[0] == '/'
                    ? root : getWorkingDirectory()); // if it's full path, start from root, else from working

    Inode *prev = nullptr, *logical_parent = nullptr, *node = nullptr;
    Inode::Status is_prev_parent;
    char path[PATH_NAME_SZ] = {0};
    strcpy(path, pathname);

    Inode::Status ret = Inode::searchTree(start, path, ext, &is_prev_parent, &prev, &logical_parent, &node);
    if(ret != Inode::FILE_EXISTS)return (ret < 0) ? ret : -1;
    // todo
    std::cout << "Need to remove: " << node->fcb->path << std::endl;
    std::cout << "Previous node: " << prev->fcb->path << std::endl;
    std::cout << "Previous is " << (is_prev_parent ? "parent\n" : "brother\n");
    std::cout << "Logical parent is: " << logical_parent->fcb->path << std::endl;
    std::cout << "Written logical parent is: " << (node->parent ? node->parent->fcb->path : "0") << std::endl;
    return 0;
}

const char *FileSystem::setWDtoParent() {
    if(working->parent) {
        setWorkingDirectory(working->parent);
    }
    return getWorkingDirectoryName();
}

void FileSystem::listWDFiles() const {
    for(Inode *next = working->child; next; next = next->bro) {
        next->printInode();
        std::cout << std::endl;
    }
}






