#pragma once
#include "TypesAndConstants.h"

class File {
public:

    static int rename(const char *path, FILE_EXT extension, const char *name);

    static int remove(const char *path, FILE_EXT extension);

    File(const char *path, FILE_EXT extension, size_t size);

    // copy
    //File(const File&);
    // File& operator=(const File&);
    // move
    // File(File&&);
    //File& operator=(File&&);
    ~File();

private:

    int open(const char *path, FILE_EXT extension, size_t size);

    int close();

    bool closed = false;
    FHANDLE handle;
};

