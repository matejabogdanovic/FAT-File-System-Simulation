#pragma once
#include "FAT.h"
#include "FileControlBlock.h"

class DirectorySystem {
public:

    ~DirectorySystem();

private:
    friend class File;

    static FHANDLE open(filename_t name, FILE_EXT extension, size_t size);

    static int close(FHANDLE file);

    static void init();

    struct Directory {

        FileControlBlock::FCB *fcb;
        Directory *left, *right;

        explicit Directory(FileControlBlock::FCB *fcb, Directory *left = nullptr, Directory *right = nullptr);

        ~Directory();
    };

    static Directory *root;
    static bool initilized;
};



