#include "../h/FileSystem.h"
#include "../h/HDisk.h"
#include "../h/File.h"
#include "../h/PrintHex.h"
#include "../h/Console.h"

/*
* This is a FAT simulation. Entry - 8bit, 2^8 blocks. Each block is 2^8*1B.
* File harddisk.txt represents disk. 
* HDisk is a singleton class which has 2 methods: read and write from a given block.
* Those are the only 2 operations which are possible to perform on a disk.
*/


// TODO: take care of 1/8th block allocation and not just from root block (LINKED ALLOCATION IN FILESYSTEM)
// [0 1 2 3 4 5 6 7] - koristiti poslednju osminu da se vodi racuna o slobodnim osminama
// koristiti control block da se vodi racuna o tome
// dinamicki?
// ======> Recimo da kazemo da maksimalno fajlova ima 256, jer 1 fajl minimalno mora zauzimati 1 ceo blok.
// posto FCB zauzima 1/8 bloka onda nam treba 256/8 = 32 blokova. Da bi vodili racuna o slobodnim osminama
// treba nam 8 bita odnosno jedan char_t. za 256 fajlova treba nam 256 char_t sto je jedan ceo blok.


void reset() {
    // FAT::clearMemory();
    FileSystem::clearRoot();
    FAT::clearFAT();
}

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
    // reset();
    std::cout << "File system init.\n";

    //
    File *f, *f1, *f2;
    try {
        // f = new File("/C", FILE_EXT::DIR, BLOCK_SZ * (256 - 4));
        // f1 = new File("/C", FILE_EXT::DIR, 1);
        //f2 = new File("/", FILE_EXT::MB, 1);
    } catch(short error) {
        std::cerr << "Error: " << std::dec << error;
        printBlocks();
        return -1;
    }

    //std::cout << std::dec << "FILE HANDLE: " << f;
    // delete f;
    // delete f1;
    File::rename("./../C/", DIR, "lolcina");
    Console::open();
    FileSystem::get().printTree();
    // std::cout << "Removing:\n" << FileSystem::get().remove("dir/f", FILE_EXT::MB) << std::endl;
    // delete f2;

    printBlocks();

    return 1;
}


