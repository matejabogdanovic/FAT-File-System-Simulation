#pragma once
#include "TypesAndConstants.h"

class File {
public:

    static int rename(const char *path, const char *name);

    static int remove(const char *path);

    File(const char *path, size_t size);

    // copy
    //File(const File&);
    // File& operator=(const File&);
    // move
    // File(File&&);
    //File& operator=(File&&);
    ~File();

private:

    int open(const char *path, size_t size);

    int close();

    bool closed = false;
    FHANDLE handle;
};

