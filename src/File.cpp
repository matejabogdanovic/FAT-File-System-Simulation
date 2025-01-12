#include "../h/File.h"
#include "../h/FileSystem.h"
#include <cstring>

File::File(const char *path) {
    int ret = FileSystem::get().open(path, 1, &handle);

    if(ret < 0)throw ret;

    if(ret == OK_FILE_IS_DIRECTORY)this->closed = true; // so can't perform file operations on directory
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

int File::read(char *data, size_t count) {
    if(closed)return -1;
    return FileSystem::get().fread(handle, count, data);
}

int File::write(const char *data, size_t count) {
    if(closed)return -1;
    return FileSystem::get().fwrite(handle, count, data);
}

int File::rename(const char *path, const char *name) {
    return FileSystem::get().rename(path, name);
}

int File::remove(const char *path) {
    return FileSystem::get().remove(path);
}

int File::getEOF() const {
    if(closed)return -1;
    uint16_t eof = 0;
    auto ret = FileSystem::get().feof(this->handle, &eof);
    if(ret < 0)return ret;
    return eof;
}

int File::getCursor() const {
    if(closed)return -1;
    uint16_t cursor = 0;
    auto ret = FileSystem::get().fcursor(this->handle, &cursor);
    if(ret < 0)return ret;
    return cursor;
}



