#pragma once
#include "TypesAndConstants.h"

class File {
public:
    int seek(uint16_t pos);

    int32_t read(char *data, size_t data_size);

    int32_t write(const char *data, size_t data_size);

    static int rename(const char *path, const char *name);

    static int remove(const char *path);

    File(const char *path);

    int32_t getEOF() const;

    int32_t getCursor() const;

    // copy
    //File(const File&);
    // File& operator=(const File&);
    // move
    // File(File&&);
    //File& operator=(File&&);
    ~File();

private:


    int close();

    bool closed = false;
    FHANDLE handle;
};

