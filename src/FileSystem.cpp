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

    return OK;
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
    FileControlBlock::printFCBt(fcb);
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
status = InodeStatus::LINK_WITH_BROTHER;\
previous = node;\
node = node->bro;

#define moveDown(status, previous, node, logical_parent)\
status = InodeStatus::LINK_WITH_PARENT;\
previous = node;\
logical_parent = previous;\
node = node->child;

#define moveUp(node, logical_parent)\
node = node->parent;\
logical_parent = node->parent;

// if FILE EXISTS, previous, status and logical parent might not be consistent
int FileSystem::searchTree(const char *path, FILE_EXT *extension,
                           InodeStatus *status, Inode **previous, Inode **logical_parent,
                           Inode **node, char *filename) {
    //  ================ check args
    if(!status || !previous || !node)return ReturnStatus::ERROR_INVALID_ARGS;
    // check if wanted directory is root
    if(!strcmp(path, "/")) {
        *node = root;
        *extension = DIR;
        *previous = nullptr;
        *logical_parent = nullptr;
        *status = InodeStatus::LINK_WITH_PARENT;
        return *status;
    }
    size_t path_len = strlen(path);
    // invalid path name length
    if(path_len > PATHNAME_SZ) return ReturnStatus::ERROR_INVALID_PATH_NAME;

    //  ================  get file name - TODO check path name for ex ////..// invalid - is it a feature :)
    char path_copy[PATHNAME_SZ + 1] = {0};
    strcpy(path_copy, path);
    if(getFileName(path_copy, extension, &path_len, filename) < 0)
        return ReturnStatus::ERROR_INVALID_PATH_NAME; // invalid name
    //std::cout << "FILE EXTENSION IS: " << file_ext_str[(*extension)] << std::endl;
    // ================ start search
    bool is_relative_path = (path[0] != '/');
    *node = (!is_relative_path ? root : getWorkingDirectory());
    *logical_parent = *previous = (*node)->parent;
    *status = InodeStatus::LINK_WITH_PARENT;
    int i = 0;
    while(true) {
        NeededFile needed_file = {strtok((i++) ? NULL : path_copy, "/")};
        bool is_needed_file_last = (needed_file.name + needed_file.len == path_len + path_copy);
        // ===================================================== SWITCH by NAME
        if(!strcmp(needed_file.name, ".")) { // =============== CASE .
            if(is_needed_file_last)
                return InodeStatus::FILE_EXISTS;
        } else if(!strcmp(needed_file.name, "..")) { // ======= CASE ..
            if((*node)->parent) { // if parent exists
                moveUp((*node), (*logical_parent))
                // *previous = *is_prev_parent = nullptr; // values don't matter
            }
            if(is_needed_file_last)
                return InodeStatus::FILE_EXISTS;
        } else { // =========================================== CASE dir or file
            moveDown((*status), (*previous), (*node), (*logical_parent))
            // moving through brothers until a match is found or not
            while(true) {
                if(!(*node))  // is null
                    return is_needed_file_last ? (*status) : ReturnStatus::ERROR_INVALID_PATH_NAME;
                // is not null
                if(is_needed_file_last) {
                    if((*node)->fcb->ext == *extension &&
                       !strcmp((*node)->fcb->name, needed_file.name)) {
                        return InodeStatus::FILE_EXISTS; // a match is found
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

int FileSystem::open(const char *pathname, size_t size, FHANDLE *file) {
    std::cout << "Opening.\n";
    block_cnt_t data_size = sizeToBlocks(size);

    // trying to find node to link current file
    Inode *prev = nullptr, *logical_parent = nullptr, *node = nullptr;
    InodeStatus linking_status;
    FILE_EXT extension;
    // if it's full path, start from root, else from working dir

    char file_name[FILENAME_SZ + 1] = {0};
    int ret = FileSystem::searchTree(pathname, &extension,
                                     &linking_status,
                                     &prev, &logical_parent, &node,
                                     file_name);

    if(node == root)return ERROR_INVALID_PATH_NAME;// can't open root
    if(ret < 0)
        return ret;
    if(ret != FILE_EXISTS && (!strcmp(file_name, ".") || !strcmp(file_name, "..")))return ERROR_INVALID_NAME;
    if(strstr(file_name, " "))return ERROR_INVALID_NAME; // can't contain blank spaces in filename


    adisk_t data_block = 0; // it's block where the data is stored
    adisk_t fcb_block = 0;

    if(ret == FILE_EXISTS) {
        std::cout << "File found!\n";
        if(node->fcb->ext == DIR) // can't open directory for writing
            return ERROR_INVALID_OPERATION;

        auto inode = (Inode *) oft.getInodeAddress(node->handle);
        if(node->isOpened() && inode == node) {
            std::cout << "File already opened. Handle: " << node->handle << std::endl;
            *file = node->handle;
            return OK;
        }
    } else {
        std::cout << "File doesn't exist. Allocating space for FCB and data block (if needed).\n";
        if(FAT::allocateInodeSpace((extension == DIR ? nullptr : &data_block),
                                   data_size,
                                   &fcb_block) < 0) {
            delete node;
            return ERROR_NO_DISK_SPACE;
        }
        std::cout << "Making inode and linking it to directory tree.\n";
        node = new Inode(new FileControlBlock::FCB(file_name, extension, data_size, data_block, fcb_block));

        // link FCBs on disk
        FileControlBlock::linkFCBs(node->fcb, prev->fcb, linking_status == LINK_WITH_PARENT);

        // link tree nodes
        Inode::linkInodes(node, prev, linking_status == LINK_WITH_PARENT, logical_parent);
    }

    if(node->fcb->ext == DIR) return OK_FILE_IS_DIRECTORY;

    std::cout << "Taking OFT entry.\n";
    *file = oft.set(
            (uint64_t) node, node->fcb->end_of_file_block * BLOCK_SZ + node->fcb->end_of_file_offs);

    node->open(*file);

    return OK;
}

int FileSystem::feof(FHANDLE file, uint16_t *eof_cursor) {
    if(file < 0)return ERROR_FILE_NOT_OPENED;
    if(!eof_cursor)return ERROR_INVALID_ARGS;// check if file is opened
    auto node = (Inode *) oft.getInodeAddress(file);
    if(!node)return ERROR_INVALID_ARGS;
    if(node->fcb->ext == DIR)return ERROR_INVALID_OPERATION;

    *eof_cursor = node->fcb->end_of_file_block * BLOCK_SZ + node->fcb->end_of_file_offs;
    return OK;
}

int FileSystem::fcursor(FHANDLE file, uint16_t *file_cursor) {
    if(!file_cursor)return ERROR_INVALID_ARGS;
    if(file < 0)return ERROR_FILE_NOT_OPENED;
    auto node = (Inode *) oft.getInodeAddress(file);
    if(!node)return ERROR_INVALID_ARGS;
    if(node->fcb->ext == DIR)return ERROR_INVALID_OPERATION;

    *file_cursor = oft.getCursor(file);; // get file cursor
    return OK;
}

int FileSystem::fseek(FHANDLE file, uint16_t pos) {
    if(file < 0)return ERROR_FILE_NOT_OPENED; // check if file is opened
    auto node = (Inode *) oft.getInodeAddress(file);
    if(!node)return ERROR_INVALID_ARGS;
    if(node->fcb->ext == DIR)return ERROR_INVALID_OPERATION;

    uint16_t eof;
    feof(file, &eof);

    if(pos > eof)pos = eof; // can't set cursor behind end of file

    return oft.setCursor(file, pos);
}

int FileSystem::f_read_or_write(FHANDLE file, size_t count, char *read_buf, const char *write_buf) {
    if(file < 0)return ERROR_FILE_NOT_OPENED;
    if((read_buf && write_buf) || (!read_buf && !write_buf))return ERROR_INVALID_ARGS;
    if(count == 0)return 0;

    auto node = (Inode *) oft.getInodeAddress(file);
    if(!node)return ERROR_INVALID_ARGS;
    if(node->fcb->ext == DIR)return ERROR_INVALID_OPERATION;

    uint16_t eof = 0;
    feof(file, &eof);
    uint64_t cursor = oft.getCursor(file);
    adisk_t data_block = node->fcb->data_block;

    // move to data block where is the cursor
    for(int i = 0; data_block != 0 && i < cursor / BLOCK_SZ; ++i) {
        // if it's writing and there is no more space in the file, extend for 1 more block
        if(write_buf && FAT::getNextBlock(data_block) == 0)
            if(!FAT::extend(data_block, 1)) {
                node->fcb->data_size += 1;
                node->changed = true;
            }
        data_block = FAT::getNextBlock(data_block);
    }

    size_t to_process = 0, processed_cnt = 0, remain_cnt = count;
    // cap the remain_cnt to end of file <=>
    // it's possible to read until the end of file, but not past it
    if(read_buf && (cursor + count > eof)) {
        remain_cnt = eof - cursor;
    }
    while(data_block != 0 && remain_cnt != 0) {
        // calculate size to read or write
        if(remain_cnt <= BLOCK_SZ - cursor % BLOCK_SZ) { // falls into this block
            to_process = remain_cnt; // write until buf[cursor + cnt]
        } else {
            to_process = BLOCK_SZ - cursor % BLOCK_SZ; // write until buf[255]
        }

        block_t buf = {0};
        HDisk::get().readBlock(buf, data_block);

        if(read_buf) {// reading
            memcpy(read_buf + processed_cnt,
                   buf + cursor % BLOCK_SZ,
                   to_process);
        } else {// writing
            memcpy(buf + cursor % BLOCK_SZ,
                   write_buf + processed_cnt,
                   to_process);
            HDisk::get().writeBlock(buf, data_block);
        }


        PrintHex::printBlock(buf, BLOCK_SZ, 16);
        std::cout << std::endl;

        processed_cnt += to_process;
        remain_cnt -= to_process;
        oft.moveCursor(file, to_process);

        // no more file space, needs to extending
        if(write_buf && (FAT::getNextBlock(data_block) == 0 && remain_cnt > 0)) {
            auto blocks_to_extend = sizeToBlocks(remain_cnt);
            if(!FAT::extend(data_block, blocks_to_extend)) {
                node->fcb->data_size += blocks_to_extend;
                node->changed = true;
            }
        }

        data_block = FAT::getNextBlock(data_block);
        cursor = oft.getCursor(file);
    }
    if(read_buf && processed_cnt > 0) {
        read_buf[processed_cnt] = 0; // set null terminator
    }

    // if cursor is larger than end of file, move end of file to cursor (more data is written)
    if(processed_cnt > 0 && cursor > (node->fcb->end_of_file_block * BLOCK_SZ + node->fcb->end_of_file_offs)) {
        node->fcb->end_of_file_offs = (uint8_t) cursor;
        node->fcb->end_of_file_block = cursor / BLOCK_SZ;

        node->changed = true;
    }
    oft.printFHANDLE(node->handle);
    return (int) processed_cnt;
}

int FileSystem::fread(FHANDLE file, size_t count, char *buf) {
    if(file < 0)return ERROR_FILE_NOT_OPENED;
    if(!buf)return ERROR_INVALID_ARGS;
    if(count == 0)return 0;

    return f_read_or_write(file, count, buf, nullptr);
}

int FileSystem::fwrite(FHANDLE file, size_t count, const char *data) {
    if(file < 0)return ERROR_FILE_NOT_OPENED;
    if(!data)return ERROR_INVALID_ARGS;
    if(count == 0)return 0;

    return f_read_or_write(file, count, nullptr, data);
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
    return OK;
}

int FileSystem::close(const char *pathname) {

    Inode *node = getExistingFile(pathname);

    if(!node || node == root)
        return ERROR_INVALID_PATH_NAME;


    return close(node);
}

int FileSystem::close(Inode *node) {
    if(node->isOpened()) {
        std::cout << "CLOSING: " << node->handle << std::endl;
        close(node->handle);
        return OK;
    }
    return ERROR_FILE_NOT_OPENED;
}

int FileSystem::rename(const char *path,
                       const char *name) {
    auto len = strlen(name);
    if(len > FILENAME_SZ)return ERROR_INVALID_NAME;

    Inode *node = getExistingFile(path);
    if(!node || node == root)
        return ERROR_INVALID_PATH_NAME;

    if(strstr(name, ".") || strstr(name, "/")) return ERROR_INVALID_NAME;

    std::cout << "Renaming " << node->fcb->name << "->" << name << std::endl;
    strcpy(node->fcb->name, name);
    node->changed = true;

    // to update working directory name
    setWorkingDirectory(getWorkingDirectory());

    return OK;
}

void FileSystem::removeRecursive(Inode *start) {
    if(!start)return;

    if(start->bro)
        removeRecursive(start->bro);

    if(start->child)
        removeRecursive(start->child);

    if(start == getWorkingDirectory())
        setWorkingDirectory(start->parent);

    if(start->fcb->ext != DIR)
        FAT::releaseBlocks(start->fcb->data_block, start->fcb->data_size);

    FAT::releaseBlocks(start->fcb->fcb_block, 1);
    // std::cout << std::dec << close(start);
    delete start;
}

int FileSystem::remove(const char *path) {
    Inode *node = getExistingFile(path);
    if(!node || node == root)return ERROR_INVALID_PATH_NAME;

    removeRecursive(node->child); // remove children
    if(node == getWorkingDirectory())
        setWorkingDirectory(node->parent);
    // deallocate this node
    if(node->fcb->ext != DIR)
        FAT::releaseBlocks(node->fcb->data_block, node->fcb->data_size);
    FAT::releaseBlocks(node->fcb->fcb_block, 1);

    close(node);

    FileControlBlock::unlinkFCBs(node->fcb, node->previous->fcb, node->parent == node->previous);
    Inode::unlinkInode(node);

    delete node;
    std::cout << "Removed.\n";
    return OK;
}

void FileSystem::clearRoot() {
    block_t block = {0};
    FileControlBlock::fcb_t fcb = {0};
    FileControlBlock::populateFCB(fcb, "/", DIR, 0, 0, ROOT_BLK);
    memcpy(block, fcb, sizeof(FileControlBlock::FCB));
    HDisk::get().writeBlock(block, ROOT_BLK);
}

int FileSystem::setWorkingDirectory(Inode *dir) {
    if(!dir || dir->fcb->ext != DIR) return ERROR_INVALID_ARGS;
    working = dir;
    working_path[0] = '\0';
    if(working == root)
        strcpy(working_path, "/");
    else
        getWorkingDirectoryPathRecursive(working, working_path);
    return OK;
}

int FileSystem::setWorkingDirectory(char *path) {
    Inode *node = getExistingFile(path);
    if(!node || node->fcb->ext != DIR)return ERROR_INVALID_PATH_NAME;


    return setWorkingDirectory(node);;
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

Inode *FileSystem::getExistingFile(const char *path) {
    // trying to find node to link current file
    Inode *prev = nullptr, *logical_parent = nullptr, *node = nullptr;
    InodeStatus linking_status;
    FILE_EXT extension;
    // if it's full path, start from root, else from working dir

    char file_name[FILENAME_SZ + 1] = {0};
    int ret = FileSystem::searchTree(path, &extension,
                                     &linking_status,
                                     &prev, &logical_parent, &node,
                                     file_name);
    if(ret != FILE_EXISTS) return nullptr;
    return node;
}

FHANDLE FileSystem::getFileHandle(const char *path) {
    if(!path)return ERROR_INVALID_ARGS;
    Inode *node = getExistingFile(path);
    if(!node)return ERROR_INVALID_PATH_NAME;
    return node->handle;
}


















