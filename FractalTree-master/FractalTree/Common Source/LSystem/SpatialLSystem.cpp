#include "pch.h"
#include "SpatialLSystem.h"
#include "..\Terrain Simulation\TerrainResources.h"

using namespace WorldGrowthSimulation;

SpatialLSystem::SpatialLSystem(WorldGrowthSimulation::TerrainResources* const terrainResources, const SimulationEngine::Actor* const actor) :
	LSystem(terrainResources, actor)
{
}

bool SpatialLSystem::Update()
{
	//can try giving generational priority to branching.  can try removing branches which don't have as much light contribution.
	const unsigned int currentSegmentListSize = static_cast<unsigned int>(this->segmentList.size());
	bool hasUpdated = false;
	unsigned int voxelsFacingSun = 1;
	for (unsigned int a = 0; a < currentSegmentListSize; ++a)
	{
		Voxelizable* const segment = this->segment(a);
		voxelsFacingSun += segment->GetUncoveredCount();
	}

	//const float resourcesPerLeaf = voxelsFacingSun / (powf((float)this->leadNodes, 2.f));
	//const float resourcesPerSegment = voxelsFacingSun / sqrtf(currentSegmentListSize);
	const float totalResources = voxelsFacingSun / (float)(currentSegmentListSize + this->leadNodes);
	const float amountPerSegment = totalResources;// *.1f;
	this->Lock();
	for (unsigned int a = 0; a < currentSegmentListSize; ++a)
	{
		SpatialLSystemSegment* const segment = this->segment(a);
		//segment->myResources += resourcesPerSegment;
		//segment->myBranchResources += resourcesPerLeaf;
		segment->myResources += amountPerSegment;
		hasUpdated |= segment->Update();
	}

	//hasUpdated |= this->grow(.35f);
	hasUpdated |= this->grow(.65f);
	this->Unlock();
	return hasUpdated;
}

bool SpatialLSystem::grow(const float amount)
{
	using namespace DirectX;

	bool changed = false;
	const unsigned int currentSegmentListSize = static_cast<unsigned int>(this->segmentList.size());
	static const unsigned int triangleCount = TRIANGLE_VOXEL_COUNT;

	for (unsigned int a = 0; a < currentSegmentListSize; ++a)
	{
		bool foundConflict = false;
		SpatialLSystemSegment* const segment = this->segment(a);
		if (segment->doneGrowing || !segment->CanAffordGrowth(amount))
		{
			continue;
		}

		XMFLOAT4 newVoxels[triangleCount];
		bool createNew[triangleCount] = { false };
		bool checkAgain[triangleCount] = { false };
		if (FractalTreeResources::checkVoxels)
		{
			const float testRadius = segment->GetTestWidthOffset() + amount;

			for (int whichTriangle = 0; whichTriangle < TRIANGLE_VOXEL_COUNT; ++whichTriangle)
			{
				//only do check if radius has increased enough to possible leave this voxel
				segment->safeDistance[whichTriangle] -= amount;
				if (segment->safeDistance[whichTriangle] > 0)
				{
					checkAgain[whichTriangle] = true;
					continue;
				}

				//need to fix this to check for the actual safest distance
				segment->safeDistance[whichTriangle] = VOXEL_SIZE / 2;

				XMVECTOR vert = XMVectorSet(cosines[whichTriangle] * testRadius,
					0,
					sines[whichTriangle] * testRadius,
					0);

				vert = XMVectorAdd(this->actor->location, XMVector3Transform(vert, segment->GetEndTransformation()));
				XMFLOAT4 vertF;
				XMStoreFloat4(&vertF, vert);
				const unsigned long long key = TerrainResources::GetKey(vertF.x, vertF.y, vertF.z);
				const TerrainResources::Voxel* vertVoxel;
				if (key == segment->lastKey[whichTriangle])
				{
					vertVoxel = segment->lastVoxel[whichTriangle];
				}
				else
				{
					createNew[whichTriangle] = true;
					vertVoxel = this->terrainResources->GetVoxel(vertF.x, vertF.y, vertF.z);
					segment->lastKey[whichTriangle] = key;
					segment->lastVoxel[whichTriangle] = vertVoxel;
				}

				newVoxels[whichTriangle] = vertF;
				if (vertVoxel == nullptr || (vertVoxel->objectType == 1 && static_cast<const SpatialLSystemSegment* const>(vertVoxel->occupier)->GetParentSystem() == this))
				{
					
				}
				else
				{
					segment->doneGrowing = true;
					foundConflict = true;
					break;
				}
			}
		}

		if (!foundConflict)
		{
			if (segment->SetTestWidthOffset(amount))
			{
				if (FractalTreeResources::checkVoxels)
				{
					for (unsigned int v = 0; v < triangleCount; ++v)
					{
						if (createNew[v])
						{
							this->terrainResources->AddVoxel(newVoxels[v].x, newVoxels[v].y, newVoxels[v].z, segment, 1);
						}
					}
				}

				changed = true;
			}
		}

	}

	return changed;
}
