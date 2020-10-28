#include "pch.h"
#include "GrowthSimulation.h"
#include "TerrainResources.h"
#include "Growable.h"

namespace WorldGrowthSimulation
{
	GrowthSimulation::GrowthSimulation() :
		terrainResources(new TerrainResources())
	{
	}

	bool GrowthSimulation::Update()
	{
		if (FractalTreeResources::enableGrowth && this->frame % 2 == 0)
		{
			if (this->growableCount > 0)
			{
				unsigned int i = 0;
				const unsigned int startInd = this->nextIndexToUpdate;
				bool doneFirst = false;

				while (i < FractalTreeResources::treeUpdatePerFrame)
				{
					if (this->nextIndexToUpdate >= this->growableObjects.size())
					{
						this->nextIndexToUpdate = 0;
					}

					if (doneFirst && startInd == this->nextIndexToUpdate)
					{
						break;
					}

					if (this->growableObjects[this->nextIndexToUpdate] == nullptr)
					{
						++this->nextIndexToUpdate;
						continue;
					}

					this->growableObjects[this->nextIndexToUpdate]->Grow();
					++this->nextIndexToUpdate;
					doneFirst = true;
					++i;
				}
			}
		}

		++this->frame;
		return true;
	}

	void GrowthSimulation::AddGrowable(Growable* const growable)
	{
		growable->SetLocalGrowableId(this->growableObjects.size());

		++this->growableCount;
		this->growableObjects.push_back(growable);
	}

	void GrowthSimulation::RemoveGrowable(Growable* const growable)
	{
		--this->growableCount;
		this->growableObjects[growable->GetLocalGrowableId()] = nullptr;
	}

	GrowthSimulation::~GrowthSimulation()
	{
		delete this->terrainResources;
	}
}
