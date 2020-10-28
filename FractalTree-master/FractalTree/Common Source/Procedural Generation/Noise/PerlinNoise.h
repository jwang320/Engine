#pragma once
#include "ProceduralGeneration.h"

namespace ProceduralGeneration
{
	class GenerationNode;

	class PerlinHeightMapGenerator
	{
	private:
		std::vector<GenerationNode*> generatorRootNodes;

	public:
		PerlinHeightMapGenerator(bool doAnything = true);		
		//Generate a height map using the set noise functions
		void GenerateHeightMap(float** heightMap, int width, int length, float gridWidth, float centerX = 0, float centerZ = 0);
		void GenerateHeightMap(float** heightMap, int width, int length, float gridWidth, float centerX, float centerZ, float& lowest, float& highest);
		float GenerateHeightValue(float x, float z) const;
	};
}