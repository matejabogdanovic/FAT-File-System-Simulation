#pragma once
#include <cstdint>



class File {
public:
	static const uint8_t FILE_NAME_SZ = 16;
	typedef const char filename_t[FILE_NAME_SZ];
	enum FILE_EXT : uint8_t {
		MB = 0xfe
	};
    File(filename_t name, FILE_EXT extension, size_t size);
    int close();
    // copy
    //File(const File&);
   // File& operator=(const File&);
    // move
   // File(File&&);
    //File& operator=(File&&);
    ~File();


private:
    friend class FAT;
    typedef int16_t FHANDLE;
    bool closed = false;
    FHANDLE handle;
};

