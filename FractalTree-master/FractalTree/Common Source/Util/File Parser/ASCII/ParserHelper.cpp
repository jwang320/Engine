#include "pch.h"
#include "ParseHelper.h"
#include <string>
#include <sstream>

void ParseUnsignedInt(std::stringstream& myFile, const std::string& delimeter, unsigned int& uIntvalue)
{
	bool foundIt = false;
	while(!foundIt)
	{
		std::string line;
		std::getline(myFile, line);
		int pos = static_cast<int>(line.find(delimeter));
		if(pos != std::string::npos)
		{
			std::string newStr = line.substr(pos + delimeter.length());
			std::istringstream intStream(newStr);
			intStream >> uIntvalue;
			foundIt = true;
		}
	}
}

char* GetFileDataAndSize2(const char* fileName, size_t& fileSize)
{
	std::ifstream myFile(fileName, std::ios::in | std::ios::binary | std::ios::ate);
	if(!myFile.is_open())
	{
		throw "file not found";//FileLoadException();
	}
	fileSize = myFile.tellg();
	myFile.seekg(0, std::ios::beg);
	char* fileData = new char[fileSize];
	myFile.read(fileData, fileSize);
	myFile.close();
	return fileData;
}