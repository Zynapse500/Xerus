#include "stdafx.h"

#include <fstream>

#include "Utility.h"

std::vector<unsigned char> xr::util::loadBytes(const char * path)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);

	std::vector<unsigned char> buffer;

	if (file.is_open()) {
		// Get the number of bytes in the file
		size_t size = file.tellg();
		file.seekg(0);

		// Resize buffer to fit file
		buffer.resize(size);

		// Map the file into memory
		file.read((char*)buffer.data(), size);
	}

	return buffer;
}
