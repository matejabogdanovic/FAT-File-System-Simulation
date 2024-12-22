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

Inode *Inode::makeNode(FileControlBlock::fcb_t fcb) {

    auto *f = (FileControlBlock::FCB *) fcb;
    auto *node = new Inode(
            new FileControlBlock::FCB(
                    *f
            )
    );

    return node;
}

int Inode::findToLink(Inode *root, Inode *node, Inode **link_to) {
    if(!node || !root || !link_to)return -10;
    if(!node->fcb || !root->fcb)return -20;


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
