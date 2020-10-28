#pragma once
#include "..\Common Source\Procedural Generation\Noise\PerlinNoise.h"

namespace WorldGrowthSimulation { class GrowthSimulation; }

namespace SimulationEngine
{
	class Terrain : public Actor
	{
	protected:
		const int cellWidth;
		float cellGridWidth;
		ProceduralGeneration::PerlinHeightMapGenerator generator;
		WorldGrowthSimulation::GrowthSimulation* const growthSimulation;

	public:
		Terrain(const ObjectParameters& objectParameters, int cellWidth, float cellGridWidth,
			const MoveableParameters& moveableParameters = MoveableParameters());
		float GetHeight(const float x, const float z) {return this->generator.GenerateHeightValue(x, z);}

		DirectX::XMVECTOR GetNormal(const float x, const float z, const float dist) const;

		virtual void Act() override;

		int getCellWidth() const { return this->cellWidth; }
		float getCellGridWidth() const { return this->cellGridWidth; }
		const ProceduralGeneration::PerlinHeightMapGenerator* GetGenerator() const { return &this->generator; }
		WorldGrowthSimulation::GrowthSimulation* GetGrowthSimulation() const { return this->growthSimulation; }
	};
}