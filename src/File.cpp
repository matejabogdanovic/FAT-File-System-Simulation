#include "../h/File.h"
#include "../h/FAT.h"
FHANDLE File::file_open(filename_t name, FILE_EXT extension, size_t size) {
    
    
    return FAT::open(name, extension, size);
}

int File::file_close(FHANDLE file) {
    
    return FAT::close(file);
}
