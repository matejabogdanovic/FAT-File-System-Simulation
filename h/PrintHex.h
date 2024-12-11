#pragma once
#include <iostream>
#include <iomanip>
#include <cmath>
class PrintHex {
public:
	static void print( int val, const char* comment = "") {
		if(comment != "")std::cout << comment << " ";
		std::cout << std::setw(2) << std::setfill('0') <<
			std::hex << val;
		std::cout << " ";
		
	}

	static void printBlock(HDisk::block_t buf, int size ,int width, int start = 0) {
		for (int i = start; i < size; i++) {
			if (i > 0 && i %  width == 0)
				std::cout << std::endl;

			PrintHex::print(buf[i]);

		}
	}

	
};