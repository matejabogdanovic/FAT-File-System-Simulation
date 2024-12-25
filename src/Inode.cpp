#include "../h/Inode.h"
#include "../h/UniversalList.h"
#include "../h/PrintHex.h"
#include "../h/FileSystem.h"
#include <cstring>

#define nextChild(link_with_parent, prev, head, log_p)\
link_with_parent = LINK_WITH_PARENT;\
prev = head;\
log_p = head;\
head = head->child;

#define nextBro(link_with_parent, prev, head)\
link_with_parent = LINK_WITH_BROTHER;\
prev = head;\
head = head->bro;

#define checkPathName(path)\
if(!strcmp(path, "/"))return ERROR_INVALID_PATH_NAME;\
auto path_len = strlen(path);\
if(path[0] != '/' && strlen(FileSystem::get().getWorkingDirectoryName()) + path_len > PATH_NAME_SZ - 1)\
return ERROR_INVALID_PATH_NAME;\
if(path_len > PATH_NAME_SZ - 1 || path[path_len - 1] == '/')return ERROR_INVALID_PATH_NAME; // no name

Inode::Inode(FileControlBlock::FCB *fcb, Inode *child, Inode *bro, Inode *parent)
        : fcb(fcb), child(child), bro(bro), parent(parent) {
}

Inode::~Inode() {
    delete fcb;
}

Inode *Inode::makeNode(FileControlBlock::fcb_t fcb) {

    auto *f = (FileControlBlock::FCB *) fcb;
    auto *node = new Inode(
            new FileControlBlock::FCB(
                    *f
            )
    );

    return node;
}

Inode::Status Inode::searchTree(Inode *start, char *path, FILE_EXT extension,
                                Status *is_prev_parent, Inode **previous, Inode **logical_parent, Inode **node) {

    if(!start || !is_prev_parent || !previous || !node)return ERROR_INVALID_ARGS;
    if(!start->fcb)return ERROR_INVALID_FCB;

    checkPathName(path)

    char needed_node[PATH_NAME_SZ] = {0};
    char pathstr[PATH_NAME_SZ] = {0};
    char path_name[PATH_NAME_SZ] = {0}; // full path even if working directory isn't really root
    strcpy(pathstr, path); // copy path to pathstr, can be relative or full path
    if(path[0] != '/') {
        // isn't full path so, find full path by concatenating working directory with path
        strcpy(path_name, start->fcb->path);
        if(strcmp(start->fcb->path, "/") != 0)  // if working isn't root => working + /
            strcat(path_name, "/");

        strcpy(needed_node, path_name); // needed_node = working/
        strcat(path_name, path); // path_name is working/ + relative path


        strcpy(path, path_name); // write full path name
    } else {
        strcpy(path_name, path);
        strcpy(needed_node, start->fcb->path); // needed_node = /
    }


    char *next_folder = strtok(pathstr, "/");

    strcat(needed_node, next_folder); // it's /+name = /name (working_dir/+name = working_dir/name )

    Inode *head = start->child, *prev = start, *log_p = start;
    Status link_status = LINK_WITH_PARENT;
    while(head) {
        if(!strcmp(head->fcb->path, needed_node)) { // path found
            // check if found file is the same as the one being opened
            if(head->fcb->ext == extension && !strcmp(head->fcb->path, path_name)) {
                *previous = prev;
                *logical_parent = log_p;
                *node = head;
                *is_prev_parent = link_status;
                return FILE_EXISTS;
            }
            // if it's not the same file, we need it to be directory at least
            if(head->fcb->ext != DIR) {
                nextBro(link_status, prev, head)
                continue;
            }

            // found directory is correct
            next_folder = strtok(NULL, "/"); // grab next directory to visit (or filename if last)

            if(!next_folder) { // path is found, just need check every file in this directory and if alright, link with bro
                nextBro(link_status, prev, head)
                continue;
            }

            strcat(needed_node, "/");
            strcat(needed_node, next_folder);

            nextChild(link_status, prev, head, log_p)
        } else { // path not found, check next bro
            nextBro(link_status, prev, head)
        }
    }
    if(strtok(NULL, "/"))return ERROR_INVALID_PATH_NAME; // INVALID PATH NAME, didn't find the wanted path
    // return values
    *previous = prev;
    *node = nullptr;
    *logical_parent = log_p;
    *is_prev_parent = link_status;
    return link_status;
}

void Inode::linkInodes(Inode *node, Inode *prev, bool is_prev_parent, Inode *logical_parent) {
    node->parent = logical_parent; // link with logical parent
    if(is_prev_parent) { // link with previous
        prev->child = node;
    } else {
        prev->bro = node;
    }
}

void Inode::printInode() {
    this->fcb->printFCB();
}

void Inode::printTree(const Inode *node, unsigned parent_name_size, int level) {

    if(level > 0) {

        for(int i = 0; i < 4 * (level - 1); ++i) {
            std::cout << " ";
        }


        std::cout << "|___";
    }


    std::cout << (node->fcb->path + parent_name_size);
    if(node->fcb->ext != DIR) std::cout << "." << file_ext_str[node->fcb->ext];

    std::cout << "\n";


    if(node->child) {
        auto len = strlen(node->fcb->path);
        printTree(node->child,
                  (parent_name_size ? len + 1 : len),
                  level + 1);
    }
    if(node->bro) {
        printTree(node->bro, parent_name_size, level);
    }
}
