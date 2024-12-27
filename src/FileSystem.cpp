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
    Inode::printTree(root);
    std::cout << "\n======Loading Tree FINISHED======\n";
    working = root;

    //root->printInode();
    //setWorkingDirectory(root->child->bro->child); // TODO REMOVE
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

int FileSystem::getFileName(const char *path, int path_len, char *file_name) {

    if(path[path_len - 1] == '/')return -1; // todo it will skip root directiory!
    const char *p = path + path_len - 1;
    while(*p != '/' && p != path) {
        p--;
    }
    if(*p == '/')p++;
    // name can't be relative path
    auto file_name_len = path_len - (p - path);
    if(file_name_len > FILENAME_SZ)return -2;
    // if(!strcmp(p, "..") || !strcmp(p, "."))return -3;

    strcpy(file_name, p);
    return 0;
}

#define nextChild(link_with_parent, prev, head, log_p)\
link_with_parent = Inode::Status::LINK_WITH_PARENT;\
prev = head;\
log_p = head;\
head = head->child;

#define nextBro(link_with_parent, prev, head)\
link_with_parent = Inode::Status::LINK_WITH_BROTHER;\
prev = head;\
head = head->bro;


struct NeededFile {
    char *name;
    bool changed = true;
    int len = 0;

    NeededFile(char *name) : name(name) {
        if(name)
            len = strlen(name);
    }
};

Inode::Status FileSystem::searchTree(const char *path, FILE_EXT extension,
                                     Inode::Status *is_prev_parent, Inode **previous, Inode **logical_parent,
                                     Inode **node, char *filename) {
    if(!is_prev_parent || !previous || !node)return Inode::Status::ERROR_INVALID_ARGS;
    if(!strcmp(path, "/")) {
        *node = root;
        *previous = nullptr;
        *logical_parent = nullptr;
        *is_prev_parent = Inode::Status::EXCEPTION_CAN_T_OPEN;
        return *is_prev_parent;
    }
    char path_copy[PATHNAME_SZ + 1] = {0};
    strcpy(path_copy, path);

    auto path_len = strlen(path_copy);
    if(path_copy[path_len - 1] == '/') {
        path_copy[--path_len] = 0;// todo check path name for ex //..// invalid
        if(path_copy[path_len - 1] == '/')return Inode::ERROR_INVALID_PATH_NAME;
    }
    if(path_len > PATHNAME_SZ)return Inode::Status::ERROR_INVALID_PATH_NAME;


    if(getFileName(path_copy, path_len, filename) < 0) {

        return Inode::Status::ERROR_INVALID_PATH_NAME;
    }
    //std::cout << "File_name is " << filename << std::endl;

    // start tokenization
    NeededFile needed_file = {strtok(path_copy, "/")};

    *node = (path[0] == '/' ? root->child : getWorkingDirectory()->child);
    *logical_parent = *previous = (path[0] == '/' ? root : getWorkingDirectory());
    *is_prev_parent = Inode::Status::LINK_WITH_PARENT;


    goto check_relative_paths;
    while((*node)) {

        if(!strcmp(needed_file.name, (*node)->fcb->path)) {// found same name

            // check if it's the last token
            if(needed_file.name +
               needed_file.len ==
               path_len +
               path_copy) { // path is found, just need check every file in this directory and if alright, link with bro
                if((*node)->fcb->ext == extension) { // if same extension
                    return Inode::Status::FILE_EXISTS;
                }
                nextBro(*is_prev_parent, *previous, (*node))
            } else { // has to be directory because it's not over
                if((*node)->fcb->ext != DIR) {
                    nextBro(*is_prev_parent, *previous, (*node))
                    continue;
                }
                // it's found ! so get next
                needed_file = NeededFile{strtok(NULL, "/")};
                nextChild(*is_prev_parent, *previous, (*node), *logical_parent)
            }
        } else {
            nextBro(*is_prev_parent, *previous, (*node))
        }


        // if . or .. move to wanted place
        check_relative_paths:
        while(needed_file.changed) {
            if(needed_file.name == nullptr) {// kinda works for .. but not for . /dir2/.
                return Inode::Status::EXCEPTION_CAN_T_OPEN;
            }
            // todo (shouldn't happen) => only if it's just .././../../..

            if(!strcmp(needed_file.name, "..")) {
                // move up but not to root
                if(*is_prev_parent == Inode::Status::LINK_WITH_PARENT) {
                    if(*previous && (*previous)->parent) {
                        *node = (*previous)->parent->child;
                        *logical_parent = (*node)->parent;
                        *previous = *logical_parent;
                        *is_prev_parent = Inode::Status::LINK_WITH_PARENT;
                    }
                } else {
                    if((*node) && (*node)->parent && (*node)->parent != root && (*node)->parent->parent) {
                        (*node) = (*node)->parent->parent->child;
                        *logical_parent = (*node)->parent;
                        *previous = *logical_parent;
                        *is_prev_parent = Inode::Status::LINK_WITH_PARENT;
                    }
                }


                needed_file = NeededFile{strtok(NULL, "/")};
            } //else if(!strcmp(needed_file.name, ".")) {
                // needed_file = NeededFile{strtok(NULL, "/")};
                // }
            else
                needed_file.changed = false;
        }
    }

    if(strtok(NULL, "/"))
        return Inode::Status::ERROR_INVALID_PATH_NAME; // INVALID PATH NAME, didn't find the wanted path
    // return values

    *node = nullptr;

    return *is_prev_parent;
}

FHANDLE FileSystem::open(const char *pathname, FILE_EXT extension, size_t size) {

    block_cnt_t data_size = sizeToBlocks(size);

    // trying to find node to link current file
    FileControlBlock::FCB *fcb = nullptr;
    Inode *prev = nullptr, *logical_parent = nullptr, *node = nullptr;
    Inode::Status is_prev_parent;
    // if it's full path, start from root, else from working dir

    char file_name[FILENAME_SZ + 1] = {0};
    Inode::Status ret = FileSystem::searchTree(pathname, extension, &is_prev_parent, &prev, &logical_parent, &node,
                                               file_name);
    if(ret < 0) {
        if(ret != Inode::Status::EXCEPTION_CAN_T_OPEN)return ret;
        node = node->parent;
    }
    printTree();
    std::cout << "Node name: " << file_name << std::endl;
    if(node)
        std::cout << "Node: " << node->fcb->path << std::endl;
    std::cout << "Previous node: " << (prev ? prev->fcb->path : "0") << std::endl;
    std::cout << "Previous is " << (is_prev_parent ? "parent\n" : "brother\n");
    std::cout << "Logical parent is: " << (logical_parent ? logical_parent->fcb->path : "0") << std::endl;
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
        fcb = new FileControlBlock::FCB(file_name, extension, data_size, data_block, fcb_block);
        node = new Inode(fcb);

        // link FCBs on disk
        FileControlBlock::linkFCBs(node->fcb, prev->fcb, is_prev_parent);

        // link tree nodes
        Inode::linkInodes(node, prev, is_prev_parent, logical_parent);
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
    working = dir;
    return 0;
}

const char *FileSystem::getWorkingDirectoryName() {
    return working->fcb->path;
}

block_cnt_t FileSystem::sizeToBlocks(size_t size) {
    return (size + BLOCK_SZ - 1) / BLOCK_SZ;
}

int FileSystem::remove(const char *pathname, FILE_EXT ext) { // todo optimize to search from working directory

    // if it's full path, start from root, else from working
    Inode *start = (pathname[0] == '/' ? root : getWorkingDirectory());
    Inode *prev = nullptr, *logical_parent = nullptr, *node = nullptr;
    Inode::Status is_prev_parent;
    char path[FILENAME_SZ + 1] = {0};
    strcpy(path, pathname);

    Inode::Status ret = Inode::searchTree(start, path, ext, &is_prev_parent, &prev, &logical_parent, &node);
    if(ret != Inode::FILE_EXISTS)return (ret < 0) ? ret : -1;
    // todo
    std::cout << "Need to remove: " << node->fcb->path << std::endl;
    std::cout << "Previous node: " << (prev ? prev->fcb->path : "0") << std::endl;
    std::cout << "Previous is " << (is_prev_parent ? "parent\n" : "brother\n");
    std::cout << "Logical parent is: " << (logical_parent ? logical_parent->fcb->path : "0") << std::endl;
    if(node && node->parent)
        std::cout << "Written logical parent is: " << node->parent->fcb->path << std::endl;
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

int FileSystem::setWDto(char *path) {
    Inode *prev = nullptr, *logical_parent = nullptr, *node = nullptr;
    Inode::Status is_prev_parent;
    // if it's full path, start from root, else from working dir

    char file_name[FILENAME_SZ + 1] = {0};
    Inode::Status ret = FileSystem::searchTree(path, DIR, &is_prev_parent, &prev, &logical_parent, &node,
                                               file_name);
    if(ret < 0 && ret != Inode::Status::EXCEPTION_CAN_T_OPEN) {
        return ret;
    }

    if(!node)return -10;
    if(ret == Inode::Status::EXCEPTION_CAN_T_OPEN && node != root)
        node = node->parent;
    setWorkingDirectory(node);
    return 0;
}







