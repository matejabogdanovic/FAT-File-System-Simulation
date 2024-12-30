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

void Inode::linkInodes(Inode *node, Inode *prev, bool is_prev_parent, Inode *logical_parent) {
    node->parent = logical_parent; // link with logical parent
    node->previous = prev;
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


    std::cout << (node->fcb->path //+ parent_name_size
    );
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


