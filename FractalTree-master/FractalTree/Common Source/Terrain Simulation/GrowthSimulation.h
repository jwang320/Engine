#pragma once
#include <vector>

namespace WorldGrowthSimulation
{
	class TerrainResources;
	class Growable;

	class GrowthSimulation
	{
	private:
		std::vector<Growable*> growableObjects;
		TerrainResources* const terrainResources;
		unsigned int growableCount = 0;
		unsigned int frame = 2;
		unsigned int nextIndexToUpdate = 0;

	public:
		GrowthSimulation();
		bool Update();
		void AddGrowable(Growable* const growable);
		void RemoveGrowable(Growable* const growable);
		~GrowthSimulation();

	public:
		TerrainResources* GetTerrainResources() { return this->terrainResources; }
	};
}
