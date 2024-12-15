#include "../h/File.h"
#include "../h/FAT.h"

File::File(filename_t name, FILE_EXT extension, size_t size){

    handle =  FAT::open(name, extension, size);
}


int File::close() {
    if(closed)return 1;
    FAT::close(handle);
    closed = true;

    return 0;
}

File::~File() {
    close();
}
