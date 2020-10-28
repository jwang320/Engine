#pragma once
#include <fstream>

void ParseUnsignedInt(std::stringstream& myFile, const std::string& delimeter, unsigned int& uIntvalue);
char* GetFileDataAndSize2(const char* fileName, size_t& fileSize);