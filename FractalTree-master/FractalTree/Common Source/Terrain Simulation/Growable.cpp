#include "pch.h"
#include "Growable.h"
#include "GrowthSimulation.h"

namespace WorldGrowthSimulation
{
	Growable::Growable(GrowthSimulation* const growthSimulation) :
		growthSimulation(growthSimulation)
	{
		this->growthSimulation->AddGrowable(this);
	}

	void Growable::removeThisGrowable()
	{
		if (!this->removed)
		{
			this->removed = true;
			this->growthSimulation->RemoveGrowable(this);
		}
	}
}
