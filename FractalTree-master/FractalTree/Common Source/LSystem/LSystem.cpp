#include "pch.h"
#include "LSystem.h"
#include "SpatialLSystemSegment.h"
#include "..\Terrain Simulation\TerrainResources.h"

LSystemSegment::LSystemSegment(LSystem* parentSystem, LSystemSegment* parent, unsigned short segmentType, unsigned short maxDepth, unsigned short currentDepth) 
	: parentSystem(parentSystem), parent(parent), maxDepth(maxDepth), currentDepth(currentDepth), segmentType(segmentType), needsIteration(false)
{
	this->parentSystem->AddSegment(this);
	++this->parentSystem->leadNodes;
	if(this->currentDepth < this->maxDepth)
	{
		this->needsIteration = true;
	}
}

LSystemSegment::~LSystemSegment()
{

}

bool LSystemSegment::Update()
{
	this->UpdateOther();
	if(this->needsIteration && this->AddChildIteration())
	{
		this->parentSystem->leadNodes -= 1;
		this->needsIteration = false;
		return true;
	}

	return false;
}

bool LSystem::UpdateLSystem()
{
	const unsigned int currentSegmentListSize = static_cast<unsigned int>(this->segmentList.size());
	bool hasUpdated = false;
	this->Lock();
	for (unsigned int a = 0; a < currentSegmentListSize; ++a)
	{
		hasUpdated |= this->segmentList[a]->Update();
	}

	this->Unlock();
	return hasUpdated;
}

LSystem::~LSystem()
{
	this->terrainResources->RemoveOwned(this);
	this->Lock();
	for (const LSystemSegment* const segment : this->segmentList)
	{
		delete segment;
	}
	this->Unlock();
}
