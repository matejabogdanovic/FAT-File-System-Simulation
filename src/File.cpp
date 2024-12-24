#include "../h/File.h"
#include "../h/FileSystem.h"
#include <cstring>

File::File(pathname_t path, FILE_EXT extension, size_t size) {
    handle = open(path, extension, size);
    if(handle < 0)throw handle;
}

int File::open(pathname_t path, FILE_EXT extension, size_t size) {
    if(size == 0 || size > BLOCK_SZ * FAT_SZ)return -2;
    if(!strcmp(path, "/"))return -3; // can't do root
    if(path[0] != '/')return -4; // isn't full path
    auto path_len = strlen(path);
    if(path_len > PATH_NAME_SZ - 1 || path[path_len - 1] == '/')return -5; // no name


    return FileSystem::get().open(path, extension, size);
}

int File::close() {
    if(closed)return 1;
    FileSystem::get().close(handle);
    closed = true;

    return 0;
}

File::~File() {
    close();
}
