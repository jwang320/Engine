#include "pch.h"
#include "Effect.h"
#include "..\PhysicalWorld.h"

namespace SimulationEngine
{
	Effect::Effect(const EffectParameters& effectParameters) :
		parentWorld(effectParameters.parentWorld),
		instant(effectParameters.instant)
	{
		this->parentWorld->AddEffect(this);
	}

	Effect::~Effect()
	{

	}
}