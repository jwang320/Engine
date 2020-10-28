#pragma once
#include "..\Common Source\Procedural Generation\Noise\PerlinNoise.h"
#include "Mesh.h"

namespace Geometry
{
	class UniformHeightMapMesh : public Mesh
	{
	public:
		float gridWidth;
		int width;
		float totalWidth;
		float xCenter;
		float zCenter;
		const ProceduralGeneration::PerlinHeightMapGenerator* generator;
		const bool useLod;
		const int terrainTextureRepeatCount;
		const bool alternateCellOrientation;

	public:
		UniformHeightMapMesh(int width, int length, float gridWidth = 30, float xCenter = 0, float zCenter = 0, bool useLod = true, int terrainTextureRepeatCount = 1, bool alternateCellOrientation = false);
		//The result will be the interpolated height at the given location.
		DirectX::XMVECTOR GetNormal(float xLoc, float zLoc, float xDist, float zDist) const;
		DirectX::XMVECTOR GetNormal(float xLoc, float zLoc, float dist) const;
		//Pass in a pre-created heightmap 2D array.
		void SetGenerator(const ProceduralGeneration::PerlinHeightMapGenerator* generator, bool generateMesh = true);
		void SetGeneratorObject(ProceduralGeneration::PerlinHeightMapGenerator* generator) {this->generator = generator;}
	private:
		int vertexCount;

		void createMultiBufferUniformPolygonMesh();
		void createFaces(int lodLevel);
		void generateBuffer();
		void generateBuffer(int lodLevel);
		void AddFace(unsigned int ind1, unsigned int ind2, unsigned int ind3, bool useFaceNormal = true, int lodIndex = 0);
		int vertCountOff[LOD_COUNT];
		//int counts[LOD_COUNT];
	public:
		int GetWidth() const {return this->width;}
		float GetGridWidth() const {return this->gridWidth;}

	};
}