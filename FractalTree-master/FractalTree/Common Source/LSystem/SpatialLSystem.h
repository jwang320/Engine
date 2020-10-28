#pragma once
#include "LSystem.h"
#include "SpatialLSystemSegment.h"

namespace WorldGrowthSimulation { class TerrainResources; }

class SpatialLSystem : public LSystem
{
public:
	SpatialLSystem(WorldGrowthSimulation::TerrainResources* const terrainResources, const SimulationEngine::Actor* const actor = nullptr);

	SpatialLSystemSegment* segment(const unsigned int index) { return static_cast<SpatialLSystemSegment*>(this->segmentList[index]); }
	bool Update();

private:
	bool grow(const float amount);
};

#define TRIANGLE_ANGLE_INCREMENT 3.1415f * 2 / TRIANGLE_VOXEL_COUNT
static const float cosines[16] = {
	cosf(TRIANGLE_ANGLE_INCREMENT * 0),
	cosf(TRIANGLE_ANGLE_INCREMENT * 1),
	cosf(TRIANGLE_ANGLE_INCREMENT * 2),
	cosf(TRIANGLE_ANGLE_INCREMENT * 3),
	cosf(TRIANGLE_ANGLE_INCREMENT * 4),
	cosf(TRIANGLE_ANGLE_INCREMENT * 5),
	cosf(TRIANGLE_ANGLE_INCREMENT * 6),
	cosf(TRIANGLE_ANGLE_INCREMENT * 7),
	cosf(TRIANGLE_ANGLE_INCREMENT * 8),
	cosf(TRIANGLE_ANGLE_INCREMENT * 9),
	cosf(TRIANGLE_ANGLE_INCREMENT * 10),
	cosf(TRIANGLE_ANGLE_INCREMENT * 11),
	cosf(TRIANGLE_ANGLE_INCREMENT * 12),
	cosf(TRIANGLE_ANGLE_INCREMENT * 13),
	cosf(TRIANGLE_ANGLE_INCREMENT * 14),
	cosf(TRIANGLE_ANGLE_INCREMENT * 15)
};

static const float sines[16] = {
	sinf(TRIANGLE_ANGLE_INCREMENT * 0),
	sinf(TRIANGLE_ANGLE_INCREMENT * 1),
	sinf(TRIANGLE_ANGLE_INCREMENT * 2),
	sinf(TRIANGLE_ANGLE_INCREMENT * 3),
	sinf(TRIANGLE_ANGLE_INCREMENT * 4),
	sinf(TRIANGLE_ANGLE_INCREMENT * 5),
	sinf(TRIANGLE_ANGLE_INCREMENT * 6),
	sinf(TRIANGLE_ANGLE_INCREMENT * 7),
	sinf(TRIANGLE_ANGLE_INCREMENT * 8),
	sinf(TRIANGLE_ANGLE_INCREMENT * 9),
	sinf(TRIANGLE_ANGLE_INCREMENT * 10),
	sinf(TRIANGLE_ANGLE_INCREMENT * 11),
	sinf(TRIANGLE_ANGLE_INCREMENT * 12),
	sinf(TRIANGLE_ANGLE_INCREMENT * 13),
	sinf(TRIANGLE_ANGLE_INCREMENT * 14),
	sinf(TRIANGLE_ANGLE_INCREMENT * 15)
};