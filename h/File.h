#pragma once
#include "TypesAndConstants.h"

class File {
public:

    File(pathname_t path, FILE_EXT extension, size_t size);

    // copy
    //File(const File&);
    // File& operator=(const File&);
    // move
    // File(File&&);
    //File& operator=(File&&);
    ~File();

private:

    int open(pathname_t path, FILE_EXT extension, size_t size);

    int close();

    bool closed = false;
    FHANDLE handle;
};

