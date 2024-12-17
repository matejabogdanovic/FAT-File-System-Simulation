#include "../h/DirectorySystem.h"
#include "../h/HDisk.h"
#include "../h/File.h"
#include "../h/PrintHex.h"


/*
* This is a FAT simulation. Entry - 8bit, 2^8 blocks. Each block is 2^8*1B.
* File harddisk.txt represents disk. 
* HDisk is a singleton class which has 2 methods: read and write from a given block.
* Those are the only 2 operations which are possible to perform on a disk.
*/


// TODO: take care of 1/8th block allocation and not just from root block
// [0 1 2 3 4 5 6 7] - koristiti poslednju osminu da se vodi racuna o slobodnim osminama
// koristiti control block da se vodi racuna o tome
// dinamicki?
// ======> Recimo da kazemo da maksimalno fajlova ima 256, jer 1 fajl minimalno mora zauzimati 1 ceo blok.
// posto FCB zauzima 1/8 bloka onda nam treba 256/8 = 32 blokova. Da bi vodili racuna o slobodnim osminama
// treba nam 8 bita odnosno jedan char_t. za 256 fajlova treba nam 256 char_t sto je jedan ceo blok.

// TODO: Change filename_t
// TODO: OFT offset ne treba? treba valjda da ima pokazivac na inode u kome je fcb, preko fcb se dodje do adrese na disku
// u koju se upisuje ili odakle se cita, da ne mora da se zna adresa na disku gde je, vec samo preko inode
int main() {

    std::cout << "File system init.\n";
    FAT::clearFAT();
    //FAT::clearMemory();
    block_t buffer;
    //
    File *f = new File("Ime", FILE_EXT::MB, 300);
    File *f2 = new File("Ime2", FILE_EXT::MB, 1);
    //std::cout << std::dec << "FILE HANDLE: " << f;
    delete f;
    delete f2;


    HDisk::get().readBlock(buffer, FAT_BLK);

    PrintHex::printBlock(buffer, BLOCK_SZ, 16);
    std::cout << std::endl;
    HDisk::get().readBlock(buffer, CONTROL_BLK);

    PrintHex::printBlock(buffer, BLOCK_SZ, 16);
}


