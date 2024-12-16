#pragma once
#include "OpenFilesTable.h"

class FAT {
public: // temporary
	/**
	 * @brief Resets FAT to 0 and occupies needed blocks.
	*/
	static void clearFAT();
	/**
	 * @brief Resets memory to 0.
	*/


	static void clearMemory();
private:
	friend class DirectorySystem;

	//static bool initilized;
	/**
	 * @brief Reads FAT and Control blocks from disk and initializes class fields.
	*/
	static void init();


    static OpenFilesTable oft;
    static fat_entry_t free_blocks_head;
    static fat_entry_t free_blocks_tail;
	static fat_t table;
    static block_t control; // [0] = free_blocks_head

	/**
	 * @brief 
	 * @param name 
	 * @param extension 
	 * @param size 
	 * @return 
	*/
	static FHANDLE open(filename_t name, FILE_EXT extension, size_t size=1);
	/**
	 * @brief 
	 * @param file 
	 * @return 
	*/
	static int close(FHANDLE file);

	/**
	 * @brief Allocates given number of blocks.
	 * @param num number of blocks to allocate
	 * @return 0 if no space
	*/
	static fat_entry_t take_blocks(block_cnt_t num);

    /**
     * @brief Frees \p num of blocks.
     * @param start first block
     * @param num number of blocks to release from \p start
     */
    static void release_blocks(adisk_t start, block_cnt_t num);
	



	
};

