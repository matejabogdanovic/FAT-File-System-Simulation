#pragma once
#include <cstdint>
#include "TypesAndConstants.h"


class File {
public:


    File(filename_t name, FILE_EXT extension, size_t size);
    int close();
    // copy
    //File(const File&);
   // File& operator=(const File&);
    // move
   // File(File&&);
    //File& operator=(File&&);
    ~File();


private:
    bool closed = false;
    FHANDLE handle;
};

