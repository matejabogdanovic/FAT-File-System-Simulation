#include "../h/FAT.h"
#include <iostream>
#include <iomanip>
#include <cstdint>

FAT::fat_entry_t FAT::free_blocks_head = 3;
FAT::fat_entry_t FAT::free_blocks_tail = 0xff;
FAT::OpenFilesTable FAT::oft;
FAT::fat_t FAT::table;
HDisk::block_t FAT::control = {0};
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
    HDisk::get().readBlock(control, CONTROL_BLK);
    free_blocks_head = control[0];
	initilized = true;

}

void FAT::OpenFilesTable::printFHANDLE(File::FHANDLE file) {
	std::cout << "\n====================";
	std::cout << std::dec << "\nFor FHANDLE: " << file << std::endl;
	PrintHex::print((block_address_t)(table[file][0] >> OFFS_SHIFT), "FCB block: ");
	std::cout << std::endl;
	PrintHex::print(table[file][0] & OFFS_MASK, "Offset in block: ");
	std::cout << "\n====================\n";
}
// TODO: Finish
int FAT::OpenFilesTable::set(block_address_t fcb_block, block_cnt_t offset_in_block) {
	int ent = take_free_entry();
    std::cout << "OFT entry taken: " << std::dec << ent;
    if(ent < 0)return -1;
    oft_entry_row_t entry = ent;
	table[entry][0] = ((oft_entry_col_t) fcb_block) << OFFS_SHIFT
		| offset_in_block;
	table[entry][1] = 0; // cursor

    FAT::OpenFilesTable::print(31);
	return entry;
}

void FAT::OpenFilesTable::print(oft_entry_row_t limit) {
    std::cout << "\n==========OFT(Block 1B, Offset 1B, Cursor 2B)==========\n";
    for(oft_entry_col_t i = 0; i <= limit; i++) {
        if (i > 0 && i % 8 == 0)
            std::cout << std::endl;
        PrintHex::print((block_address_t)(table[i][0] >>  OFFS_SHIFT));
        PrintHex::print(table[i][0] & OFFS_MASK);
        PrintHex::print(table[i][1]);

    }
    if(limit != OFT_SZ-1)
        std::cout << "...";
    std::cout << "\n=======================================================\n";
}

int FAT::OpenFilesTable::take_free_entry() {
    int entry = -1;
    for (int i = 0; i < FREE_V_SZ; ++i) {
        if(free_vector[i] == UINT64_MAX)continue;
        // there is free entry
        uint64_t mask = 0x1;
        int j = 0;
        for (; j <64; ++j) {
            if((free_vector[i] & mask)== 0){
                // found
                free_vector[i] |= mask;
                break;
            }

            mask<<=1;
        }
        entry = 64*i + j;
        break;
    }

    return entry;
}

void FAT::OpenFilesTable::free_entry(File::FHANDLE fhandle) {
    auto i = fhandle/64;
    auto j = fhandle%64;

    free_vector[i] &= ~(uint64_t)(1<<j);
}

// finds free blocks, allocates space, makes FCB for the file
// returns an entry from table of open files
File::FHANDLE FAT::open(File::filename_t name, File::FILE_EXT extension, size_t size) {
	if (size == 0 || size > HDisk::BLOCK_SZ * FAT::FAT_SZ)return -1;
	
	block_cnt_t data_size = (size + HDisk::BLOCK_SZ - 1) / HDisk::BLOCK_SZ;
	fat_entry_t entry_index = take_free_blocks(data_size);
	if (entry_index == 0)return -2;


	fcb_t buf;
	populateFCB(buf, name, extension, data_size, entry_index);

	printFCB(buf);
	
	return oft.set(entry_index, 0); // TODO: implement offset_in_block
}


int FAT::close(File::FHANDLE file) {

    std::cout << "\n=====CLOSING=====";
    std::cout << "\n=====FREE OFT=====\n";
    oft.free_entry(file);
    std::cout << "\n=====FAT=>DISK[0]=====\n";
    HDisk::get().writeBlock(table, FAT_BLK);
    std::cout << "\n=====CONTROL=>DISK[1]=====\n";
    control[0] = free_blocks_head;
    HDisk::get().writeBlock(control, CONTROL_BLK);
    oft.printFHANDLE(file);
//    std::cout << "\n=====CLOSED=====\n FHANDLE: " << file << "\n FAT: \n";
//
//    PrintHex::printBlock(table, 256, 16);
//    std::cout << "\n================\n";
	return 0;
}

// returns index in FAT of the first free block
// invalid returns 0
FAT::fat_entry_t FAT::take_free_blocks(block_cnt_t num) {
	if (free_blocks_head == 0)return 0;
	if (!initilized)init();


	fat_entry_t start = free_blocks_head, last = free_blocks_head;

	for (uint16_t i = 0; i < num; i++) { 
		if (free_blocks_head == 0) { 
			free_blocks_head = start; // there is not enough space to allocate, so reset  
			break; 
		}
        last = free_blocks_head;
		free_blocks_head = table[free_blocks_head];
	}
    if(free_blocks_head == start)return 0;

    table[last] = 0;

	return start;
}


void FAT::clearFATandCONTROL() {
	uint8_t buf[HDisk::BLOCK_SZ];
	control[0] = 3;

    buf[FAT_BLK] = 0; // fat block
	buf[CONTROL_BLK] = 0; // for storing fat data on disk
	buf[ROOT_BLK] = 0; // root directory

	for(uint16_t i = 3; i < HDisk::BLOCK_SZ; i++) {
		buf[i] = (uint8_t)i+1;
	}
	
	HDisk::get().writeBlock(buf, FAT_BLK);
    HDisk::get().writeBlock(control, CONTROL_BLK);
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

void FAT::free_blocks(block_address_t start, block_cnt_t num) {

}





