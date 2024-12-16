#include "../h/File.h"
#include "../h/DirectorySystem.h"


File::File(filename_t name, FILE_EXT extension, size_t size) {
    handle = DirectorySystem::open(name, extension, size);
}

int File::close() {
    if(closed)return 1;
    DirectorySystem::close(handle);
    closed = true;

    return 0;
}

File::~File() {
    close();
}
