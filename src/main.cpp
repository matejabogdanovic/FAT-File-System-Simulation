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
	FAT::clearFAT();
	//FAT::clearMemory();
	HDisk::block_t buffer;

	FHANDLE f = File::file_open("Ime", File::FILE_EXT::MB, 1);
	std::cout << std::dec << "FILE HANDLE: " << f;
	File::file_close(f);

	HDisk::get().readBlock(buffer, 0);
	
	PrintHex::printBlock(buffer, HDisk::BLOCK_SZ, 16);

}