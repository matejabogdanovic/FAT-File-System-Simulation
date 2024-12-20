#include "../h/Inode.h"
#include "../h/UniversalList.h"
#include "../h/PrintHex.h"
#include <cstring>

#define nextChild(link_with_parent, prev, head)\
link_with_parent = true;\
prev = head;\
head = head->child;

#define nextBro(link_with_parent, prev, head)\
link_with_parent = false;\
prev = head;\
head = head->bro;


Inode::Inode(FileControlBlock::FCB *fcb, Inode *child, Inode *bro)
        : fcb(fcb), child(child), bro(bro) {
}

Inode::~Inode() {
    delete fcb;
}

Inode *Inode::make_node(FileControlBlock::fcb_t fcb) {

    auto *f = (FileControlBlock::FCB *) fcb;
    auto *node = new Inode(
            new FileControlBlock::FCB(
                    *f
            )
    );

    return node;
}

// TODO: split by / and make sure that it's in order
int Inode::findToLink(Inode *root, Inode *node, Inode **link_to) {
    if(!node || !root || !link_to)return -10;
    if(!node->fcb)return -20;


    char needed_node[PATH_NAME_SZ] = {0};
    char pathstr[PATH_NAME_SZ] = {0};
    strcpy(pathstr, node->fcb->path);
    strcpy(needed_node, "/");
    char *next_folder = strtok(pathstr, "/");
    strcat(needed_node, next_folder); // it's /+name = /name

    Inode *head = root->child, *prev = root;
    bool link_with_parent = true;
    while(head) {
        if(!strcmp(head->fcb->path, needed_node)) { // path found
            // check if found file is the same as the one being opened
            if(head->fcb->ext == node->fcb->ext && !strcmp(head->fcb->path, node->fcb->path)) {
                *link_to = head;
                return 2; // FILE ALREADY EXISTS
            }
            // if it's not the same file, we need it to be directory at least
            if(head->fcb->ext != DIR) {
                nextBro(link_with_parent, prev, head)
                continue;
            }

            // found directory is correct
            next_folder = strtok(NULL, "/"); // grab next directory to visit (or filename if last)

            if(!next_folder) { // path is found, just need check every file in this directory and if alright, link with bro
                nextBro(link_with_parent, prev, head)
                continue;
            }

            strcat(needed_node, "/");
            strcat(needed_node, next_folder);

            nextChild(link_with_parent, prev, head)
        } else { // path not found, check next bro
            nextBro(link_with_parent, prev, head)
        }
    }
    if(strtok(NULL, "/"))return -40; // INVALID PATH NAME, didn't find the wanted path
    // return values
    *link_to = prev;
    return link_with_parent;
//
//    Inode *head = root, *prev = root;
//    char *next_folder = nullptr;
//    bool link_with_parent = true, path_finished = false;
//    for(int i = 0; head; ++i) {
//        prev = head;
//        if(!strcmp(head->fcb->path, token)) { // found
//            if(!strcmp(head->fcb->path, node->fcb->path)) {
//                if(head->fcb->ext == node->fcb->ext) {
//                    *link_to = head;
//                    return 2; // FILE ALREADY EXISTS
//                } else { // extensions not the same but path is
//
//                }
//            }
//
//            if(head->fcb->ext != DIR) {
//                link_with_parent = false;
//                head = head->bro; // check next file in current directory
//                continue;
//            }
//
//
//            if(i == 0) {
//                // if it's root folder, start with string tokenization
//                next_folder = strtok(pathstr, "/");
//            } else {
//                next_folder = strtok(NULL, "/");
//                if(!next_folder) {
//                    // the whole path is found which means, this hit happened because path names are the same
//                    // but extensions aren't
//                    // TODO problem, can't break here, what if the file exists somewhere forward
//                    // we need to get to the last file and link with it
//                    link_with_parent = false;
//                    continue;
//                }
//
//                strcat(token, "/");
//            }
//
//            strcat(token, next_folder);
//
//            link_with_parent = true;
//            head = head->child; // go deeper
//        } else {
//            link_with_parent = false;
//            head = head->bro; // check next file in current directory
//        }
//    }
//    if(strtok(NULL, "/"))return -40; // INVALID PATH NAME, didn't find wanted path
////    if(link_with_parent) { DON't link here, just return information if linking is possible
////        prev->child = node;
////    } else {
////        prev->bro = node;
////    }
//
//    // return values
//    *link_to = prev;
//    return link_with_parent;
}

int Inode::link(bool link_with_parent, Inode *node, Inode *prev) {
    if(!node || !prev)return -1;
    if(link_with_parent) {
        prev->child = node;
    } else {
        prev->bro = node;
    }
    return 0;
}
