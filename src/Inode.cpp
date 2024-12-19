#include "../h/Inode.h"
#include "../h/UniversalList.h"
#include "../h/PrintHex.h"
#include <cstring>

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
int Inode::add(Inode *root, Inode *node, Inode **link_to) {
    if(!node || !root || !link_to)return -10;
    if(!node->fcb)return -20;


    char token[PATH_NAME_SZ] = {0};
    char pathstr[PATH_NAME_SZ] = {0};
    strcpy(pathstr, node->fcb->path);
    strcpy(token, "/");


    Inode *head = root, *prev = root;
    char *next_folder = nullptr;
    bool link_with_parent = true;
    while(head) {
        prev = head;
        if(!strcmp(head->fcb->path, token)) { // found
            if(head->fcb->ext == node->fcb->ext && !strcmp(head->fcb->path, node->fcb->path)) {
                *link_to = head;
                return 2; // FILE ALREADY EXISTS
            }

            if(head->fcb->ext != DIR) {
                link_with_parent = false;
                head = head->bro; // check next file in current directory
                continue;
            }

            // return -5; // INVALID PATHNAME, (File already exists or) used filename as pathname

            if(strcmp(head->fcb->path, "/") == 0) { // TODO: optimize
                // if it's root folder, start with string tokenization
                next_folder = strtok(pathstr, "/");
            } else {
                strcat(token, "/");
                next_folder = strtok(NULL, "/");
                if(!next_folder)break; //TODO problem?
            }

            strcat(token, next_folder);

            link_with_parent = true;
            head = head->child; // go deeper
        } else {
            link_with_parent = false;
            head = head->bro; // check next file in current directory
        }
    }
    if(strtok(NULL, "/"))return -40; // INVALID PATH NAME, didn't find wanted path
    if(link_with_parent) {
        prev->child = node;
    } else {
        prev->bro = node;
    }

    // return values
    *link_to = prev;
    return link_with_parent;
}

