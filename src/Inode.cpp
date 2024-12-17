#include "../h/Inode.h"

Inode::Inode(FileControlBlock::FCB *fcb, Inode *left, Inode *right)
        : fcb(fcb), left(left), right(right) {
}

Inode::~Inode() {
    delete fcb;
}

Inode *Inode::make_node(FileControlBlock::fcb_t fcb) {

    auto *f = (FileControlBlock::FCB *) fcb;
    auto *dir = new Inode(
            new FileControlBlock::FCB(
                    *f
            )
    );

    return dir;
}
