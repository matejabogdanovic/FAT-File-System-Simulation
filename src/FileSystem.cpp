#include "../h/FileSystem.h"
#include "../h/PrintHex.h"
#include "../h/HDisk.h"
#include <cstring>


FileSystem &FileSystem::get() {
    static FileSystem dir;
    return dir;
}

int FileSystem::init() {

    std::cout << "======Loading Tree======\n";
    root = FileSystem::loadTree(ROOT_BLK);
    if(!root)return -1;
    Inode::printTree(root);
    std::cout << "======Loading Tree FINISHED======\n";
    setWorkingDirectory(root);

    return 0;
}

FileSystem::FileSystem() {
    FAT::init();

    if(FileSystem::init() < 0)throw -1;
}

void FileSystem::deallocateTree(Inode *start, bool save_to_disk) {
    if(!start)return;

    if(start->bro)
        deallocateTree(start->bro, true);

    if(start->child)
        deallocateTree(start->child, true);

    start->printInode();
    std::cout << std::endl;
    if(start->isOpened()) {
        std::cout << "Not closed - closing: " << start->handle << std::endl;
        get().close(start);
    }
    if(save_to_disk && start->changed) {
        std::cout << "Dirty - saving to disk." << std::endl;
        FileControlBlock::saveFCBtoDisk(start->fcb);
    }
    delete start;
}

FileSystem::~FileSystem() {

    std::cout << "~~~~~~~~~Deallocating Tree~~~~~~~~~\n";
    deallocateTree(root);

    FAT::saveToDisk();
}

// todo should be done with block + offset
Inode *FileSystem::loadTree(adisk_t block, Inode *logical_parent, Inode *previous) {
    block_t fcb = {0};
    HDisk::get().readBlock(fcb, block);
    //FileControlBlock::printFCBt(fcb);
    Inode *node = Inode::makeNode(fcb);
    if(!node)return nullptr;
    if(logical_parent) {
        node->parent = logical_parent;
    }
    if(previous) {
        node->previous = previous;
    }

    if(node->fcb->bro)
        node->bro = loadTree(node->fcb->bro, logical_parent, node);

    if(node->fcb->child)
        node->child = loadTree(node->fcb->child, node, node);

    return node;
}

int FileSystem::getFileName(char *path, FILE_EXT *extension, size_t *path_len, char *file_name) {
    bool has_to_be_dir = false;
    if(path[(*path_len) - 1] == '/') { // if last character is / then it must be ..../dir/
        has_to_be_dir = true; // error if ..../file/ - can't treat file like directory
        path[--(*path_len)] = 0; // ..../dir/ => ..../dir
        if(path[*path_len - 1] == '/')return -1; // if ....// error because no directory name!
    }
    // now find file name going from back of the path to first /
    const char *p = path + (*path_len) - 1;
    char *ext = path + (*path_len) - 1;
    while(true) {
        if(*p != '/' && p != path)
            p--;
        if(*ext != '.' && *ext != '/' && ext != path)
            ext--;
        if(!(*p != '/' && p != path) && !(*ext != '.' && *ext != '/' && ext != path))
            break;
    }
    if(*p == '/')p++; // position p at the beginning of name
    if(!strcmp(p, ".") || !strcmp(p, "..")) {
        *extension = DIR;
    } else {
        if(*ext == '.') {
            *ext = 0;
            ext++;
            (*path_len) = ext - path - 1;
            int i = 0;
            for(; i < FILE_EXT_CNT; ++i) {
                if(!strcmp(file_ext_str[i], ext)) {
                    *extension = (FILE_EXT) i;
                    break;
                }
            }
            if(i == FILE_EXT_CNT)return -10;
        } else if(*ext == '/' || ext == path)
            *extension = DIR;
        else
            return -11;
    }
    if(has_to_be_dir && (*extension) != DIR)return -1;


    auto file_name_len = (*path_len) - (p - path);
    // invalid file name length

    if(file_name_len > FILENAME_SZ)return -2;

    strcpy(file_name, p);
    return 0;
}

struct NeededFile {
    char *name;
    int len = 0;

    NeededFile(char *name) : name(name) {
        if(name)
            len = strlen(name);
    }
};

#define moveRight(status, previous, node)\
status = Inode::Status::LINK_WITH_BROTHER;\
previous = node;\
node = node->bro;

#define moveDown(status, previous, node, logical_parent)\
status = Inode::Status::LINK_WITH_PARENT;\
previous = node;\
logical_parent = previous;\
node = node->child;

#define moveUp(node, logical_parent)\
node = node->parent;\
logical_parent = node->parent;

// if FILE EXISTS, previous, status and logical parent might not be consistent
Inode::Status FileSystem::searchTree(const char *path, FILE_EXT *extension,
                                     Inode::Status *status, Inode **previous, Inode **logical_parent,
                                     Inode **node, char *filename) {
    //  ================ check args
    if(!status || !previous || !node)return Inode::Status::ERROR_INVALID_ARGS;
    // check if wanted directory is root
    if(!strcmp(path, "/")) {
        *node = root;
        *extension = DIR;
        *previous = nullptr;
        *logical_parent = nullptr;
        *status = Inode::Status::LINK_WITH_PARENT;
        return *status;
    }
    size_t path_len = strlen(path);
    // invalid path name length
    if(path_len > PATHNAME_SZ) return Inode::Status::ERROR_INVALID_PATH_NAME;

    //  ================  get file name - TODO check path name for ex ////..// invalid - is it a feature :)
    char path_copy[PATHNAME_SZ + 1] = {0};
    strcpy(path_copy, path);
    if(getFileName(path_copy, extension, &path_len, filename) < 0)
        return Inode::Status::ERROR_INVALID_PATH_NAME; // invalid name
    std::cout << "FILE EXTENSION IS: " << file_ext_str[(*extension)] << std::endl;
    // ================ start search
    bool is_relative_path = (path[0] != '/');
    *node = (!is_relative_path ? root : getWorkingDirectory());
    *logical_parent = *previous = (*node)->parent;
    *status = Inode::Status::LINK_WITH_PARENT;
    int i = 0;
    while(true) {
        NeededFile needed_file = {strtok((i++) ? NULL : path_copy, "/")};
        bool is_needed_file_last = (needed_file.name + needed_file.len == path_len + path_copy);
        // ===================================================== SWITCH by NAME
        if(!strcmp(needed_file.name, ".")) { // =============== CASE .
            if(is_needed_file_last)
                return Inode::Status::FILE_EXISTS;
        } else if(!strcmp(needed_file.name, "..")) { // ======= CASE ..
            if((*node)->parent) { // if parent exists
                moveUp((*node), (*logical_parent))
                // *previous = *is_prev_parent = nullptr; // values don't matter
            }
            if(is_needed_file_last)
                return Inode::Status::FILE_EXISTS;
        } else { // =========================================== CASE dir or file
            moveDown((*status), (*previous), (*node), (*logical_parent))
            // moving through brothers until a match is found or not
            while(true) {
                if(!(*node))  // is null
                    return is_needed_file_last ? (*status) : Inode::Status::ERROR_INVALID_PATH_NAME;
                // is not null
                if(is_needed_file_last) {
                    if((*node)->fcb->ext == *extension &&
                       !strcmp((*node)->fcb->name, needed_file.name)) {
                        return Inode::Status::FILE_EXISTS; // a match is found
                    }
                    moveRight((*status), (*previous), (*node))
                } else {
                    if((*node)->fcb->ext == DIR &&
                       !strcmp((*node)->fcb->name, needed_file.name)) {
                        break; // a match is found <3
                    }
                    moveRight((*status), (*previous), (*node))
                }
            }
        }
    }
}

FHANDLE FileSystem::open(const char *pathname, size_t size) {

    block_cnt_t data_size = sizeToBlocks(size);

    // trying to find node to link current file
    Inode *prev = nullptr, *logical_parent = nullptr, *node = nullptr;
    Inode::Status is_prev_parent;
    FILE_EXT extension;
    // if it's full path, start from root, else from working dir

    char file_name[FILENAME_SZ + 1] = {0};
    Inode::Status ret = FileSystem::searchTree(pathname, &extension,
                                               &is_prev_parent,
                                               &prev, &logical_parent, &node,
                                               file_name);
    if(node == root)return -9;// can't open root
    if(ret < 0)
        return ret;
    if(ret != Inode::FILE_EXISTS && (!strcmp(file_name, ".") || !strcmp(file_name, "..")))return -8;
    if(strstr(file_name, " "))return -9; // can't contain blank spaces in filename

    printTree();
    std::cout << "Node name: " << file_name << std::endl;
    if(node)
        std::cout << "Node: " << node->fcb->name << std::endl;
    std::cout << "Previous node: " << (prev ? prev->fcb->name : "0") << std::endl;
    std::cout << "Previous node written: " << (node && node->previous ? node->previous->fcb->name : "0") << std::endl;
    std::cout << "Previous is " << (is_prev_parent ? "parent\n" : "brother\n");
    std::cout << "Logical parent is: " << (logical_parent ? logical_parent->fcb->name : "0") << std::endl;
    if(node && node->parent)
        std::cout << "Written logical parent is: " << node->parent->fcb->name << std::endl;

    std::cout << "======Linking with ";

    adisk_t data_block = 0; // it's block where the data is stored
    adisk_t fcb_block = 0;

    if(ret == Inode::FILE_EXISTS) {
        std::cout << "no one======\n";
        std::cout << "ALREADY EXISTS.\n";
        auto inode = (Inode *) oft.getInodeAddress(node->handle);
        if(node->isOpened() && inode == node) {
            std::cout << "ALREADY OPENED: " << node->handle << std::endl;
            return node->handle;
        }
    } else {
        std::cout << (is_prev_parent ? "parent======\n" : "brother======\n");

        if(FAT::allocateFileSpace(&data_block, data_size, &fcb_block) < 0) {
            delete node;
            return -1;
        }

        node = new Inode(new FileControlBlock::FCB(file_name, extension, data_size, data_block, fcb_block));

        // link FCBs on disk
        FileControlBlock::linkFCBs(node->fcb, prev->fcb, is_prev_parent);

        // link tree nodes
        Inode::linkInodes(node, prev, is_prev_parent, logical_parent);
    }


    // usually we would need FCB address to write in OFT but since it's write back, we will directly write in data block

    //if(fcb->ext == DIR)return -69; // todo can't open oft entry for directory + dont allocate data block etc
    FHANDLE handle = oft.set(
            (uint64_t) node);

    node->open(handle);

    return handle;
}

int FileSystem::close(FHANDLE file) {
    std::cout << "=====CLOSING: " << file << "=====\n";
    std::cout << "=====FREE OFT=====\n";
    oft.printFHANDLE(file);

    auto inode = (Inode *) oft.getInodeAddress(file);
    inode->close();

    oft.releaseEntry(file);
    std::cout << "OFT entry freed.\n";

    std::cout << "=====CLOSED: " << file << "=====\n";
    return 0;
}

int FileSystem::close(const char *pathname) {
    Inode *prev = nullptr, *logical_parent = nullptr, *node = nullptr;
    Inode::Status is_prev_parent;
    FILE_EXT extension;
    // if it's full path, start from root, else from working dir

    char file_name[FILENAME_SZ + 1] = {0};
    Inode::Status ret = FileSystem::searchTree(pathname, &extension, &is_prev_parent, &prev, &logical_parent, &node,
                                               file_name);
    if(ret != Inode::FILE_EXISTS || node == root)
        return -1;


    return close(node);
}

int FileSystem::close(Inode *node) {
    if(node->isOpened()) {
        std::cout << "CLOSING: " << node->handle << std::endl;
        close(node->handle);
        return 0;
    }
    return -2;
}

int FileSystem::rename(const char *path,
                       const char *name) {
    if(strlen(name) > FILENAME_SZ)return -1;
    Inode *prev = nullptr, *logical_parent = nullptr, *node = nullptr;
    Inode::Status is_prev_parent;
    FILE_EXT extension;
    // if it's full path, start from root, else from working dir

    char file_name[FILENAME_SZ + 1] = {0};
    Inode::Status ret = FileSystem::searchTree(path, &extension, &is_prev_parent, &prev, &logical_parent, &node,
                                               file_name);
    if(ret != Inode::FILE_EXISTS || node == root)
        return -2;

    std::cout << "Renaming " << node->fcb->name << "->" << name << std::endl;
    strcpy(node->fcb->name, name);
    node->changed = true;

    // to update working directory name
    setWorkingDirectory(getWorkingDirectory());

    return 0;
}

void FileSystem::removeRecursive(Inode *start) {
    if(!start)return;

    if(start->bro)
        removeRecursive(start->bro);

    if(start->child)
        removeRecursive(start->child);

    if(start == getWorkingDirectory())
        setWorkingDirectory(start->parent);

    FAT::releaseBlocks(start->fcb->data_block, start->fcb->data_size);
    FAT::releaseBlocks(start->fcb->fcb_block, 1);
    std::cout << std::dec << close(start);
    delete start;
}

int FileSystem::remove(const char *path) {
    Inode *prev = nullptr, *logical_parent = nullptr, *node = nullptr;
    Inode::Status is_prev_parent;
    FILE_EXT extension;
    char file_name[FILENAME_SZ + 1] = {0};
    Inode::Status ret = FileSystem::searchTree(path, &extension,
                                               &is_prev_parent,
                                               &prev, &logical_parent, &node,
                                               file_name);
    if(node == root)return -8;
    if(ret != Inode::FILE_EXISTS)return (ret < 0) ? ret : -1;

    std::cout << "NEEDS REMOVING\n";
    std::cout << "Node name: " << file_name << std::endl;
    if(node)
        std::cout << "Node: " << node->fcb->name << std::endl;
    std::cout << "Previous node: " << (prev ? prev->fcb->name : "0") << std::endl;
    std::cout << "Previous node written: " << (node && node->previous ? node->previous->fcb->name : "0") << std::endl;
    std::cout << "Previous is " << (is_prev_parent ? "parent\n" : "brother\n");
    std::cout << "Logical parent is: " << (logical_parent ? logical_parent->fcb->name : "0") << std::endl;
    if(node && node->parent)
        std::cout << "Written logical parent is: " << node->parent->fcb->name << std::endl;


    removeRecursive(node->child); // remove children
    if(node == getWorkingDirectory())
        setWorkingDirectory(node->parent);
    // deallocate this node
    FAT::releaseBlocks(node->fcb->data_block, node->fcb->data_size);
    FAT::releaseBlocks(node->fcb->fcb_block, 1);

    close(node);

    FileControlBlock::unlinkFCBs(node->fcb, node->previous->fcb, node->parent == node->previous);
    Inode::unlinkInode(node);

    delete node;

    // to prevent deleting directory you are already in
    // setWorkingDirectory(root);

    return 0;
}

void FileSystem::clearRoot() {
    block_t block = {0};
    FileControlBlock::fcb_t fcb = {0};
    FileControlBlock::populateFCB(fcb, "/", DIR, 0, 0, ROOT_BLK);
    memcpy(block, fcb, sizeof(FileControlBlock::FCB));
    HDisk::get().writeBlock(block, ROOT_BLK);
}

int FileSystem::setWorkingDirectory(Inode *dir) {
    if(!dir || dir->fcb->ext != DIR) return -1;
    working = dir;
    working_path[0] = '\0';
    if(working == root)
        strcpy(working_path, "/");
    else
        getWorkingDirectoryPathRecursive(working, working_path);
    return 0;
}

int FileSystem::setWorkingDirectory(char *path) {
    Inode *prev = nullptr, *logical_parent = nullptr, *node = nullptr;
    Inode::Status is_prev_parent;
    FILE_EXT extension;
    // if it's full path, start from root, else from working dir

    char file_name[FILENAME_SZ + 1] = {0};
    Inode::Status ret = FileSystem::searchTree(path, &extension, &is_prev_parent, &prev, &logical_parent, &node,
                                               file_name);
    if(ret < 0) {
        return ret;
    }

    if(!node || extension != DIR)return -10;

    setWorkingDirectory(node);
    return 0;
}

void FileSystem::printTree() const {
    Inode::printTree(root);
}

Inode *FileSystem::getWorkingDirectory() const {
    return working;
}

const char *FileSystem::getWorkingDirectoryName() const {
    return working->fcb->name;
}

void FileSystem::getWorkingDirectoryPathRecursive(Inode *node, char *path) const {
    if(node->parent)
        getWorkingDirectoryPathRecursive(node->parent, path);
    if(node != root) {

        strcat(path, "/");
        strcat(path, node->fcb->name);
    }
}

const char *FileSystem::getWorkingDirectoryPath() const {
    return working_path;
}

void FileSystem::listWorkingDirectory() const {
    for(Inode *next = working->child; next; next = next->bro) {
        next->printInode();
        std::cout << std::endl;
    }
}

void FileSystem::clearMemory() {
    block_t buf = {0};

    for(uint16_t i = 0; i < BLOCK_SZ; i++) {
        HDisk::get().writeBlock(buf, i);
    }
}

block_cnt_t FileSystem::sizeToBlocks(size_t size) {
    return (size + BLOCK_SZ - 1) / BLOCK_SZ;
}






