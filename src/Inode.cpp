#include "../h/Inode.h"
#include "../h/Queue.h"
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
// level order search
int Inode::add(Inode *root, Inode *node, Inode **link_to) {
    if(!node || !root || !link_to)return -1;
    if(!node->fcb)return -2;
    if(node->fcb->path[0] != '/')return -3;

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
            if(head->fcb->ext != DIR)return -5; // INVALID PATHNAME, File already exists or used filename as pathname

            if(strcmp(head->fcb->path, "/") == 0) {
                // if it's root folder, start with string tokenization
                next_folder = strtok(pathstr, "/");
            } else {
                strcat(token, "/");
                next_folder = strtok(NULL, "/");
            }

            strcat(token, next_folder);

            link_with_parent = true;
            head = head->child; // go deeper
        } else {
            link_with_parent = false;
            head = head->bro; // check next file in current directory
        }
    }
    //auto name = strtok(pathstr, "/");
    // if name == null then the path is found
    // if name is not null,
    if(strtok(NULL, "/"))return -4; // INVALID PATH NAME, didn't find wanted path
    //if(!strcmp(token, prev->fcb->path))return -5; // ALREADY EXISTS
    if(link_with_parent) {
        prev->child = node;
    } else {
        prev->bro = node;
    }

    // return values
    *link_to = prev;
    return link_with_parent;
//    while(token != NULL) {
//        std::cout << token << std::endl;
//        token = strtok(NULL, "/");
//    }
//    Queue<Inode *> queue;
//    Inode *head = nullptr;
//
//    queue.addLast(root);
//    queue.addLast(nullptr);
//
//    unsigned level = 1;
//    bool level_switched = true;
//    while(queue.size()) {
//        if(queue.peekFirst() == nullptr) {
//            queue.removeFirst(); // remove nullptr
//            if(!queue.size()) // no more nodes to visit
//                break;
//            level++;
//            level_switched = true;
//            queue.addLast(nullptr);
//            continue;
//        }
//
//        head = queue.removeFirst();
//
//        if(!strcmp(head->fcb->path, token)) { // found
//            if(!head->child) { // needs to be last folder
//                auto name = strtok(NULL, "/"); // needs to be filename
//                if(strtok(NULL, "/") != nullptr)return -4; // INVALID PATH NAME
//            }
//            queue.clear();
//            level_switched = true;
//        }
//
//        // visit
//
//        if(level_switched) { // only the 1st one from a new level enters here
//            level_switched = false;
//            while(true) {
//            }
//        }
//    }
}

