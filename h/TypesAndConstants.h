#pragma once
#include <cstdint>

// DISK
static const uint16_t BLOCK_SZ = 256; // FAT is stored in exactly 1 block.

typedef uint8_t adisk_t; // 8 bits to address one block
typedef uint8_t char_t; // addressibile unit in one block is one
typedef char_t block_t[BLOCK_SZ];
typedef uint16_t dchar_t; // double char
typedef uint8_t block_cnt_t;
static const adisk_t FAT_BLK = 0;
static const adisk_t CONTROL_BLK = 1;
static const adisk_t ROOT_BLK = 2;
// FAT
static const auto FAT_SZ = BLOCK_SZ;
typedef adisk_t fat_entry_t;
typedef fat_entry_t fat_t[FAT_SZ];
//FILE
static const uint8_t FILENAME_SZ = 15;
static const uint16_t PATHNAME_SZ = 256;
//typedef char filename_t[FILENAME_SZ];
//typedef const char pathname_t[FILENAME_SZ - 1];
enum FILE_EXT : uint8_t {
    error = 0xFF,
    MB = 0x01,
    DIR = 0x00
};
static const auto FILE_EXT_CNT = 2;
static const char *file_ext_str[FILE_EXT_CNT] = {"dir", "mb"};
typedef int16_t FHANDLE;


// FCB


// OFT
static const auto OFT_SZ = FAT_SZ;
//static const uint8_t OFFS_SHIFT = sizeof(uint32_t) * 8 / 2;
//static const dchar_t OFFS_MASK = 0xFF;
static const auto FREE_V_SZ = OFT_SZ / (sizeof(uint64_t) * 8);
typedef fat_entry_t aoft_t; // address in oft
typedef uint64_t oft_t[OFT_SZ][2]; // [block | offset_in_block] [        cursor       ]

