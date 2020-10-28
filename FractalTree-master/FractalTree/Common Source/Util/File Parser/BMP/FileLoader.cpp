#include "pch.h"
#include "FileLoader.h"

namespace FileLoader
{
	int** LoadBMP(const char* fileData, int dataSize, int& imgWidth, int& imgHeight)
	{
		char h[2];
		long long currentIndex = 0;

		memcpy(h, fileData + currentIndex, 2);
		currentIndex+= 2;

		int fileSize;
		memcpy(&fileSize, fileData + currentIndex, sizeof(int));
		currentIndex+= sizeof(int) + 4;

		int pixelStart;
		memcpy(&pixelStart, fileData + currentIndex, sizeof(int));
		currentIndex+= sizeof(int);

		//bytes in structure:
		currentIndex+= sizeof(int);

		memcpy(&imgWidth, fileData + currentIndex, sizeof(int));
		currentIndex+= sizeof(int);

		memcpy(&imgHeight, fileData + currentIndex, sizeof(int));
		currentIndex+= sizeof(int);

		unsigned short planeCount; 
		memcpy(&planeCount, fileData + currentIndex, sizeof(unsigned short));
		currentIndex+= sizeof(unsigned short);

		unsigned short bpp;
		memcpy(&bpp, fileData + currentIndex, sizeof(unsigned short));
		currentIndex+= sizeof(unsigned short);

		int compression;
		memcpy(&compression, fileData + currentIndex, sizeof(int));
		currentIndex+= sizeof(int);
			
		//useless stuff
		//currentIndex+= sizeof(int) * 5;
		currentIndex = pixelStart;
		int** mapData = new int*[imgWidth];
		for(int x = 0; x < imgWidth; ++x)
		{
			mapData[x] = new int[imgHeight];
			for(int z = 0; z < imgHeight; ++z)
			{
				unsigned char B, G, R;// , A;
				memcpy(&B, fileData + currentIndex, sizeof(char));
				currentIndex+= 1;
				memcpy(&G, fileData + currentIndex, sizeof(char));
				currentIndex+= 1;
				memcpy(&R, fileData + currentIndex, sizeof(char));
				currentIndex+= 1;
				/*memcpy(&A, fileData + currentIndex, sizeof(char));
				currentIndex+= 1;
				*/
				mapData[x][z] = R;
			}
		}

		return mapData;
	}

	float** LoadBMPFloat(const char* fileData, int dataSize, long int& imgWidth, long int& imgHeight)
	{
		char h[2];
		long long currentIndex = 0;

		memcpy(h, fileData + currentIndex, 2);
		currentIndex+= 2;

		long int fileSize;
		memcpy(&fileSize, fileData + currentIndex, sizeof(long int));
		currentIndex+= sizeof(long int) + 4;

		long int pixelStart;
		memcpy(&pixelStart, fileData + currentIndex, sizeof(long int));
		currentIndex+= sizeof(long int);

		//bytes in structure:
		currentIndex+= sizeof(long int);

		memcpy(&imgWidth, fileData + currentIndex, sizeof(long int));
		currentIndex+= sizeof(long int);

		memcpy(&imgHeight, fileData + currentIndex, sizeof(long int));
		currentIndex+= sizeof(long int);

		unsigned short planeCount; 
		memcpy(&planeCount, fileData + currentIndex, sizeof(unsigned short));
		currentIndex+= sizeof(unsigned short);

		unsigned short bpp;
		memcpy(&bpp, fileData + currentIndex, sizeof(unsigned short));
		currentIndex+= sizeof(unsigned short);

		long int compression;
		memcpy(&compression, fileData + currentIndex, sizeof(long int));
		currentIndex+= sizeof(long int);

		//useless stuff
		currentIndex+= sizeof(long int) * 5;

		float** mapData = new float*[imgWidth];
		for(int x = 0; x < imgWidth; ++x)
		{
			mapData[x] = new float[imgHeight];
			for(int z = 0; z < imgHeight; ++z)
			{
				unsigned char B, G, R, A;
				memcpy(&B, fileData + currentIndex, sizeof(char));
				currentIndex+= 1;
				memcpy(&G, fileData + currentIndex, sizeof(char));
				currentIndex+= 1;
				memcpy(&R, fileData + currentIndex, sizeof(char));
				currentIndex+= 1;
				memcpy(&A, fileData + currentIndex, sizeof(char));
				currentIndex+= 1;

				mapData[x][z] = R;
			}
		}

		return mapData;
	}

}