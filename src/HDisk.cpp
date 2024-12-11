#include "../h/HDisk.h"
#include <iostream>
#include <iomanip>
#include <fstream>


HDisk::HDisk() {
    fopen_s(&disk, "../src/harddisk.txt", "r+b");
    
    std::cout << "Disk opened.\n";
}

int HDisk::readBlock(block_t buf, block_address_t blck_num) {
    fseek(disk, blck_num * BLOCK_SZ, SEEK_SET);

    std::cout << std::dec << "Read " << fread(buf, sizeof(uint8_t), BLOCK_SZ, disk) 
              << " from block " << (int)blck_num << std::endl;
    return 0;
}

int HDisk::writeBlock(block_t buf, block_address_t blck_num) {
    fseek(disk, blck_num * BLOCK_SZ, SEEK_SET);

    std::cout << std::dec << "Written " << fwrite(buf, sizeof(uint8_t), BLOCK_SZ, disk)
              << " in block " << (int)blck_num << std::endl;
    return 0;
}



HDisk& HDisk::get() {
    static HDisk hdisk;
    return hdisk;
}

HDisk::~HDisk() {
    fclose(disk);
    std::cout << "\nDisk closed.\n";


}
