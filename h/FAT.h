#pragma once
#include "HDisk.h"
#include "File.h"
#include "PrintHex.h"

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
	friend class File;

	/* const */

	static const block_address_t FAT_BLK = 0;
	static const block_address_t CONTROL_BLK = 1;
	static const block_address_t ROOT_BLK = 2;
	static const auto FAT_SZ = HDisk::BLOCK_SZ;

	/* typdef */

	typedef block_address_t fat_entry_t; 
	typedef fat_entry_t fat_t[FAT_SZ];

	/**
	 * @brief File control block. 
	*/
	struct  FCB { // 32 chars => 8 FCBs in ONE block
		File::filename_t name; 
		File::FILE_EXT ext;
		block_cnt_t data_size;
		fat_entry_t entry;
		block_address_t left, right;

		block_au_t padding[11];

	};
	typedef block_au_t fcb_t[sizeof(FCB)];

	/**
	 * @brief Populates \p buf with given parameters.
	 * @param buf 
	 * @param name 
	 * @param extension 
	 * @param data_size 
	 * @param entry_index 
	*/
	static void populateFCB(fcb_t buf, 
		File::filename_t name,
		File::FILE_EXT extension,
		block_cnt_t data_size,
		fat_entry_t entry_index);
	/**
	 * @brief Prints \p fcb using a predefined format.
	 * @param fcb 
	*/
	static void printFCB(fcb_t fcb);

	static bool initilized;
	/**
	 * @brief Reads FAT from disk and initilizes table.
	*/
	static void init();

	/**
	 * @brief 
	*/
	class OpenFilesTable {
		friend class FAT;
		typedef fat_entry_t oft_entry_row_t; // entry for row
		typedef uint16_t oft_entry_col_t; // entry for column (real entry)
		typedef oft_entry_col_t oft_t[FAT::FAT_SZ][2];
		static const oft_entry_col_t OFFS_MASK = 0x01;
		/**
		 * @brief Prints \p file using a predefined format.
		 * @param file 
		*/
		void print(FHANDLE file);
		/**
		 * @brief Occupies one free entry in the table.
		 * @param fcb_block 
		 * @param offset_in_block 
		 * @return index of occupied entry
		*/
		int set(block_address_t fcb_block, block_au_t offset_in_block);

		oft_t table;
		oft_entry_row_t free_entry = 0;
	};

	static OpenFilesTable oft;
	static fat_entry_t free_blocks_head;
	static fat_t table;

	/**
	 * @brief 
	 * @param name 
	 * @param extension 
	 * @param size 
	 * @return 
	*/
	static FHANDLE open(File::filename_t name, File::FILE_EXT extension, size_t size);
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
	static fat_entry_t find_free_blocks(block_cnt_t num);


	



	
};

