#pragma once
#include <cstdint>

typedef int16_t FHANDLE;

class File {
public:
	static const uint8_t FILE_NAME_SZ = 16;
	typedef const char filename_t[FILE_NAME_SZ];
	enum FILE_EXT : uint8_t {
		MB = 0xfe
	};
	// If -1, then error
	static FHANDLE file_open(filename_t name, FILE_EXT extension, size_t size);
	static int file_close(FHANDLE file);
	
};

