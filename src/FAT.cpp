#include "../h/FAT.h"
#include <iostream>
#include <iomanip>
#include <cstdint>

uint8_t FAT::free_blocks_head = 3;
FAT::OpenFilesTable FAT::oft;
FAT::fat_t FAT::table;
bool FAT::initilized = false;

void FAT::populateFCB(fcb_t buf,
	File::filename_t name,
	File::FILE_EXT extension,
	block_cnt_t data_size,
	fat_entry_t entry_index) {
	bool name_finished = false;
	size_t i = 0;
	for (size_t j = 0; j < sizeof(FCB::name); j++, i++) {

		if (name[i] == '\0')name_finished = true;
		buf[i] = (name_finished ? 0 : name[i]);

	}
	for (size_t j = 0; j < sizeof(FCB::ext); j++, i++) {
		buf[i] = extension;

	}
	for (size_t j = 0; j < sizeof(FCB::data_size); j++, i++) {
		buf[i] = data_size;

	}
	for (size_t j = 0; j < sizeof(FCB::entry); j++, i++) {
		buf[i] = entry_index;

	}
	for (size_t j = 0; j < sizeof(FCB::left); j++, i++) {
		buf[i] = 1;

	}
	for (size_t j = 0; j < sizeof(FCB::right); j++, i++) {
		buf[i] = 2;

	}
	for (size_t j = 0; j < sizeof(FCB::padding); j++, i++) {
		buf[i] = 15;

	}
}

void FAT::printFCB(fcb_t buf) {

	std::cout << "\n====================";
	std::cout << "\nFile name: ";
	for (size_t i = 0; i < sizeof(FCB::name); i++) {
		if (buf[i] != '\0')std::cout << buf[i];
	}

	std::cout << "\nFCB block:\n";
	PrintHex::printBlock(buf, sizeof(fcb_t), 8);
	std::cout << "\n====================\n";
}

void FAT::init() {

	HDisk::get().readBlock(table, FAT_BLK);
	initilized = true;

}

void FAT::OpenFilesTable::print(FHANDLE file) {
	std::cout << "\n====================";
	std::cout << std::dec << "\nFor FHANDLE: " << file << std::endl;
	PrintHex::print((block_address_t)table[file][0] >> sizeof(block_cnt_t), "FCB block: ");
	std::cout << std::endl;
	PrintHex::print(table[file][0] & OFFS_MASK, "Offset in block: ");
	std::cout << "\n====================\n";
}
// TODO: Finish
int FAT::OpenFilesTable::set(block_address_t fcb_block, block_cnt_t offset_in_block) {
	oft_entry_row_t entry = free_entry;
	
	table[entry][0] = ((oft_entry_col_t) fcb_block) << sizeof(offset_in_block)
		| offset_in_block;
	table[entry][1] = 0; // cursor

	free_entry++;

	return entry;
}

// finds free blocks, allocates space, makes FCB for the file
// returns an entry from table of open files
FHANDLE FAT::open(File::filename_t name, File::FILE_EXT extension, size_t size) {
	if (size == 0 || size > HDisk::BLOCK_SZ * FAT::FAT_SZ)return -1;
	
	block_cnt_t data_size = (size + HDisk::BLOCK_SZ - 1) / HDisk::BLOCK_SZ;
	fat_entry_t entry_index = find_free_blocks(data_size);
	if (entry_index == 0)return -2;


	fcb_t buf;
	populateFCB(buf, name, extension, data_size, entry_index);

	printFCB(buf);
	
	return oft.set(table[entry_index], 0);
}


int FAT::close(FHANDLE file) {
	oft.print(file);
	return 0;
}

// returns index in FAT of the first free block
// invalid returns 0
FAT::fat_entry_t FAT::find_free_blocks(block_cnt_t num) {
	if (free_blocks_head == 0)return 0;
	if (initilized == false)init();


	fat_entry_t start = free_blocks_head;
	for (uint16_t i = 0; i < num; i++) { 
		if (free_blocks_head == 0) { 
			free_blocks_head = start; // there is not enough space to allocate, so reset  
			break; 
		}
		free_blocks_head = table[free_blocks_head];

	}

	return (free_blocks_head==start ? 0:start);
}


void FAT::clearFAT() {
	uint8_t buf[HDisk::BLOCK_SZ];
	buf[FAT_BLK] = 0; // fat block
	buf[CONTROL_BLK] = 0; // for storing fat data on disk
	buf[ROOT_BLK] = 0; // root directory

	for(uint16_t i = 3; i < HDisk::BLOCK_SZ; i++) {
		buf[i] = (uint8_t)i+1;
	}
	
	HDisk::get().writeBlock(buf, FAT_BLK);

}

void FAT::clearMemory() {
	HDisk::block_t buf;
	for (uint16_t i = 0; i < HDisk::BLOCK_SZ; i++) {
		buf[i] = (uint8_t)0;
	}
	for (uint16_t i = 0; i < HDisk::BLOCK_SZ; i++) {
		HDisk::get().writeBlock(buf, i);
	}

}



