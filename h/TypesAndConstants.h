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


enum ReturnStatus : int {
    OK_FILE_IS_DIRECTORY = 1, // special value for open
    OK = 0,
    ERROR_INVALID_ARGS = -1,
    ERROR_INVALID_PATH_NAME = -2,
    ERROR_INVALID_NAME = -3,
    ERROR_NO_DISK_SPACE = -4,
    ERROR_FILE_NOT_OPENED = -5,
    ERROR_INVALID_OPERATION = -6,
};
static const char *errorMessage[6] = {
        "Error: Invalid arguments.",
        "Error: Invalid path name.",
        "Error: Invalid file name.",
        "Error: No disk space available.",
        "Error: File not opened.",
        "Error: Invalid operation."
};

static const char *getErrorMessage(ReturnStatus status) {
    if(status >= 0)return nullptr;
    int index = -status - 1;
    if(index > 5)return nullptr;
    return errorMessage[index];
}

enum InodeStatus : int {
    LINK_WITH_BROTHER = 1,
    LINK_WITH_PARENT = 2,
    FILE_EXISTS = 3
};
// OFT
static const auto OFT_SZ = FAT_SZ;
//static const uint8_t OFFS_SHIFT = sizeof(uint32_t) * 8 / 2;
//static const dchar_t OFFS_MASK = 0xFF;
static const auto FREE_V_SZ = OFT_SZ / (sizeof(uint64_t) * 8);
typedef fat_entry_t aoft_t; // address in oft
typedef uint64_t oft_t[OFT_SZ][2]; // [block | offset_in_block] [        cursor       ]

