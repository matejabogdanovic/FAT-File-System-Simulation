#include "../h/File.h"
#include "../h/Console.h"

/*
* This is a FAT simulation. Entry - 8bit, 2^8 blocks. Each block is 2^8*1B.
* File harddisk.txt represents disk. 
* HDisk is a singleton class which has 2 methods: read and write from a given block.
* Those are the only 2 operations which are possible to perform on a disk.
*/


void reset() {
    //FileSystem::clearMemory(); // uncomment this to wipe memory and set to 0
    FileSystem::clearRoot();
    FAT::clearFAT();
}


// include these so printBlocks work
#include "../h/HDisk.h"
#include "../h/PrintHex.h"

// prints FAT, CONTROL and ROOT block
void printBlocks() {

    block_t buffer;
    HDisk::get().readBlock(buffer, FAT_BLK);

    PrintHex::printBlock(buffer, BLOCK_SZ, 16);
    std::cout << std::endl;
    HDisk::get().readBlock(buffer, CONTROL_BLK);

    PrintHex::printBlock(buffer, BLOCK_SZ / 4, 16);
    std::cout << "..." << std::endl;
    HDisk::get().readBlock(buffer, ROOT_BLK);

    PrintHex::printBlock(buffer, BLOCK_SZ / 4, 16);
    std::cout << "..." << std::endl;
}

int main() {
    //reset(); // uncomment this to reset the memory
    std::cout << "File system init.\n";
    File *f;
    try {
        f = new File("y.mb");
    } catch(int error) {
        std::cerr << "Error: " << std::dec << error;
        printBlocks();
        return -1;
    }
    // writing
    f->seek(0);
    f->write("123", 3);
    // reading the whole file
    auto eof = f->getEOF();
    if(eof < 0)return -1;
    char *read_data = new char[eof + 1]{0}; // + 1 for null terminator
    f->seek(0);
    f->read(read_data, f->getEOF());
    std::cout << "Data is: " << read_data << std::endl;
    delete[] read_data;
    // renaming
    File::rename("./../y.mb", "file");
    delete f;

    // start console
    Console::open();
    FileSystem::get().printTree();

    return OK;
}


