#pragma once
#include "Effect.h"

namespace SimulationEngine
{
	struct SpatialEffectParameters
	{
		unsigned int validTargetTypes;
	};

	const SpatialEffectParameters DefaultSpatialEffectParameters = { 0 };

	//SpatialEffect is a class of effect that applies an effect to eligible objects
	//in its bounded area.
	class SpatialEffect : public Effect
	{
	protected:
		unsigned int validTargetTypes;

	public:
		SpatialEffect(const EffectParameters& effectParameters, const SpatialEffectParameters& spatialEffectParameters = DefaultSpatialEffectParameters);
		virtual void Apply() override;

	protected:
		//ApplyEffectToTarget will be called on any object the effect should target.
		//This method must be overriden in sub-classes.  The sub-class's implementation
		//must decide what type of object to cast the target to (for example, a force effect
		//should cast the target parameter to a Moveable).
		virtual void ApplyEffectToTarget(Actor* target) = 0;
	};
}