#include "../h/File.h"
#include "../h/DirectorySystem.h"


File::File(pathname_t path, FILE_EXT extension, size_t size) {
    handle = DirectorySystem::open(path, extension, size);
    if(handle < 0)throw handle;
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
