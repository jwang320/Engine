#pragma once

namespace WorldGrowthSimulation
{
	class Voxelizable
	{
	public:
		mutable unsigned int verticalCoverCount = 0; //the number of voxels above this one in the same vertical column
		mutable unsigned int totalVoxelCount = 0;

		unsigned int GetUncoveredCount() const 
		{ 
			if (this->totalVoxelCount > this->verticalCoverCount)
				return this->totalVoxelCount - this->verticalCoverCount;
			return 0; 
		}
	};
}