#include "../h/FAT.h"
#include "../h/PrintHex.h"
#include <iostream>
#include <iomanip>
#include <cstdint>
/*
* This is a FAT simulation. Entry - 8bit, 2^8 blocks. Each block is 2^8*1B.
* File harddisk.txt represents disk. 
* HDisk is a singleton class which has 2 methods: read and write from a given block.
* Those are the only 2 operations which are possible to perform on a disk.
*/


int main() {

	std::cout << "File system init.\n";
    FAT::clearFileSystem();
	//FAT::clearMemory();
	block_t buffer;
//
	File* f = new File("Ime", FILE_EXT::MB, 1);
    File* f2= new File("Ime2", FILE_EXT::MB, 1);
//	//std::cout << std::dec << "FILE HANDLE: " << f;
    delete f;
   delete f2;


	HDisk::get().readBlock(buffer, 0);
	
	PrintHex::printBlock(buffer, BLOCK_SZ, 16);
    std::cout << std::endl;
    HDisk::get().readBlock(buffer, 1);

    PrintHex::printBlock(buffer, BLOCK_SZ, 16);

}


