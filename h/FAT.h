#pragma once
#include "HDisk.h"
#include "File.h"
#include "PrintHex.h"

class FAT {
public: // temporary
	/**
	 * @brief Resets FAT to 0 and occupies needed blocks.
	*/
	static void clearFATandCONTROL();
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
        static const uint16_t OFT_SZ = FAT::FAT_SZ;
		typedef fat_entry_t oft_entry_row_t; // entry for row
		typedef uint16_t oft_entry_col_t; // entry for column (real entry)
        static const uint8_t OFFS_SHIFT = sizeof(oft_entry_col_t)*8/2;
		typedef oft_entry_col_t oft_t[OFT_SZ][2]; // [block | offset_in_block] [        cursor       ]
		static const oft_entry_col_t OFFS_MASK = 0xFF;

        static const auto FREE_V_SZ = OFT_SZ/(sizeof(uint64_t)*8);

        OpenFilesTable(){
            for(oft_entry_col_t i = 0; i < OFT_SZ; i++) {
                table[i][0] = i+1;
                table[i][1] = (oft_entry_col_t)0;

            }

        }
        /**
         * @brief Print up to \p limit row.
         * @param limit
         */
        void print(oft_entry_row_t limit = 255);
		/**
		 * @brief Prints \p file using a predefined format.
		 * @param file 
		*/
		void printFHANDLE(File::FHANDLE file);
		/**
		 * @brief Occupies one free entry in the table.
		 * @param fcb_block 
		 * @param offset_in_block 
		 * @return index of occupied entry
		*/
		int set(block_address_t fcb_block, block_au_t offset_in_block);
        /**
         * @brief
         * @return negative if no free entries
         */
        int take_free_entry();

        void free_entry(File::FHANDLE fhandle);

		oft_t table{};
		uint64_t free_vector[FREE_V_SZ] = {0};
	};

	static OpenFilesTable oft;
    static fat_entry_t free_blocks_head;
    static fat_entry_t free_blocks_tail;
	static fat_t table;
    static HDisk::block_t control; // [0] = free_blocks_head

	/**
	 * @brief 
	 * @param name 
	 * @param extension 
	 * @param size 
	 * @return 
	*/
	static File::FHANDLE open(File::filename_t name, File::FILE_EXT extension, size_t size=1);
	/**
	 * @brief 
	 * @param file 
	 * @return 
	*/
	static int close(File::FHANDLE file);

	/**
	 * @brief Allocates given number of blocks.
	 * @param num number of blocks to allocate
	 * @return 0 if no space
	*/
	static fat_entry_t take_free_blocks(block_cnt_t num);

    static void free_blocks(block_address_t start, block_cnt_t num);
	



	
};

