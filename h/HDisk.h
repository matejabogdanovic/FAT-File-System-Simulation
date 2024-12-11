#pragma once
#include "stdio.h"
#include <cstdint>

typedef uint8_t block_address_t; // 8 bits to address one block
typedef uint8_t block_au_t; // addressibile unit in one block is one 
typedef uint8_t block_cnt_t;
class HDisk {
public:
	static const uint16_t BLOCK_SZ = 256; // FAT is stored in exactly 1 block.
	typedef block_au_t block_t[BLOCK_SZ];
	/**
	 * @brief Reads a block from \p blck_num address and writes it in \p buf.
	 * @param buf 
	 * @param blck_num 
	 * @return 
	*/
	int readBlock(block_t buf, block_address_t blck_num = 0);
	/**
	 * @brief Writes a block from \p buf to \p blck_num address on a disk.
	 * @param buf 
	 * @param blck_num 
	 * @return 
	*/
	int writeBlock(block_t buf, block_address_t blck_num = 0);
	/**
	 * @brief 
	 * @return a static reference to HDisk 
	*/
	static HDisk& get();
	~HDisk();
private:
	FILE* disk;
	HDisk();
};

