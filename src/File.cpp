#include "../h/File.h"
#include "../h/FileSystem.h"
#include <cstring>

File::File(const char *path, size_t size) {
    handle = open(path, size);
    if(handle < 0)throw handle;
}

int File::open(const char *path, size_t size) {
    if(size == 0 || size > BLOCK_SZ * FAT_SZ)return -2;

    return FileSystem::get().open(path, size);
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

int File::seek(uint16_t pos) {
    if(closed)return -1;
    return FileSystem::get().fseek(this->handle, pos);
}

int32_t File::write(const char *data, size_t data_size) {
    if(closed)return -1;
    return FileSystem::get().fwrite(handle, data_size, data);
}

int File::rename(const char *path, const char *name) {
    return FileSystem::get().rename(path, name);
}

int File::remove(const char *path) {
    return FileSystem::get().remove(path);
}

int32_t File::getEOF() const {
    if(closed)return -1;
    uint16_t eof = 0;
    auto ret = FileSystem::get().feof(this->handle, &eof);
    if(ret < 0)return ret;
    return eof;
}

int32_t File::getCursor() const {
    if(closed)return -1;
    uint16_t cursor = 0;
    auto ret = FileSystem::get().fcursor(this->handle, &cursor);
    if(ret < 0)return ret;
    return cursor;
}


