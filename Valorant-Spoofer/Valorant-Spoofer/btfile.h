#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

namespace Memory
{
	bool ReadFileM(const std::string& file_path, std::vector<uint8_t>* out_buffer);
	bool CreateFileM(const std::string& desired_file_path, const char* address, size_t size);
}