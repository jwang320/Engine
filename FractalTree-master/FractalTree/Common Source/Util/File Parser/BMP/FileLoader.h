#pragma once

namespace FileLoader
{
	int** LoadBMP(const char* fileData, int dataSize, int& imgWidth, int& imgHeight);
	float** LoadBMPFloat(const char* fileData, int dataSize, long int& imgWidth, long int& imgHeight);
}