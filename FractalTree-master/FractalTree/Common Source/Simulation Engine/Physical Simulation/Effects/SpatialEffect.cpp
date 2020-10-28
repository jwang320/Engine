#include "pch.h"
#include "SpatialEffect.h"
#include "..\PhysicalWorld.h"

namespace SimulationEngine
{
	SpatialEffect::SpatialEffect(const EffectParameters& effectParameters, const SpatialEffectParameters& spatialEffectParameters) :
		Effect(effectParameters),
		validTargetTypes(spatialEffectParameters.validTargetTypes)
	{

	}

	void SpatialEffect::Apply()
	{
		for(int a = 0; a < this->parentWorld->GetObjectCount(); ++a)
		{
			if(this->parentWorld->IsObjectRegistered(this->parentWorld->GetObjectGlobalID(a), this->validTargetTypes))
			{
				this->ApplyEffectToTarget(static_cast<Actor*>(this->parentWorld->GetObjectByLocalID(a)));
			}		
		}
	}
}