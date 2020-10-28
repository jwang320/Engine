#include "pch.h"
#include "Terrain.h"
#include "..\..\..\Common Source\Terrain Simulation\GrowthSimulation.h"

using namespace DirectX;
using namespace ProceduralGeneration;
using namespace WorldGrowthSimulation;

namespace SimulationEngine
{
	Terrain::Terrain(const ObjectParameters& objectParameters, int cellWidth, float cellGridWidth, const MoveableParameters& moveableParameters) :
		Actor(objectParameters, moveableParameters),
		cellWidth(cellWidth),
		cellGridWidth(cellGridWidth),
		generator(PerlinHeightMapGenerator(true)),
		growthSimulation(new GrowthSimulation())
	{
		this->objectType = ObjectTypes::Terrain;
		this->componentFlags |= PhysicalWorld::ACT;
	}

	void Terrain::Act()
	{
		this->growthSimulation->Update();
	}

	XMVECTOR Terrain::GetNormal(float x, float z, float dist) const
	{
		const float h0 = this->generator.GenerateHeightValue(x, z - dist);
		const float h1 = this->generator.GenerateHeightValue(x - dist, z);
		const float h2 = this->generator.GenerateHeightValue(x + dist, z);
		const float h3 = this->generator.GenerateHeightValue(x, z + dist);

		const XMVECTOR normal = XMVectorSet(h1 - h2, 2 * dist, h0 - h3, 0);
		return XMVector3Normalize(normal);
	}
}