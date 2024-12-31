#include "../h/Inode.h"
#include "../h/PrintHex.h"
#include "../h/FileSystem.h"
#include <cstring>


Inode::Inode(FileControlBlock::FCB *fcb, Inode *child, Inode *bro, Inode *parent, Inode *previous)
        : fcb(fcb), child(child), bro(bro), parent(parent), previous(previous) {
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

void Inode::unlinkInode(Inode *node) {
    bool prev_is_parent = (node->previous == node->parent);
    if(prev_is_parent) {
        node->previous->child = node->bro;
    } else {
        node->previous->bro = node->bro;
    }
    node->previous->changed = true;

    if(node->bro && node->bro->previous == node) {
        node->bro->previous = node->previous;
        node->bro->changed = true;
    }
}

void Inode::linkInodes(Inode *node, Inode *prev, bool is_prev_parent, Inode *logical_parent) {
    node->parent = logical_parent; // link with logical parent
    node->previous = prev;
    if(is_prev_parent) { // link with previous
        prev->child = node;
    } else {
        prev->bro = node;
    }
    node->changed = true;
    node->previous->changed = true;
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


    std::cout << (node->fcb->name //+ parent_name_size
    );
    if(node->fcb->ext != DIR) std::cout << "." << file_ext_str[node->fcb->ext];

    std::cout << "\n";


    if(node->child) {
        auto len = strlen(node->fcb->name);
        printTree(node->child,
                  (parent_name_size ? len + 1 : len),
                  level + 1);
    }
    if(node->bro) {
        printTree(node->bro, parent_name_size, level);
    }
}

bool Inode::isOpened() const {
    return handle >= 0;
}

void Inode::open(FHANDLE fhandle) {
    if(fhandle < 0)return;
    handle = fhandle;
}

void Inode::close() {
    handle = -1;
}


