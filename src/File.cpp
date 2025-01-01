#include "../h/File.h"
#include "../h/FileSystem.h"
#include <cstring>

File::File(const char *path, FILE_EXT extension, size_t size) {
    handle = open(path, extension, size);
    if(handle < 0)throw handle;
}

int File::open(const char *path, FILE_EXT extension, size_t size) {
    if(size == 0 || size > BLOCK_SZ * FAT_SZ)return -2;

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

int File::rename(const char *path, FILE_EXT extension, const char *name) {
    return FileSystem::get().rename(path, extension, name);
}

int File::remove(const char *path, FILE_EXT extension) {
    return FileSystem::get().remove(path, extension);
}

