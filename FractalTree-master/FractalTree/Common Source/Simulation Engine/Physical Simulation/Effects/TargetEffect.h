#pragma once
#include "Effect.h"

namespace SimulationEngine
{
	/*enum EffectTargetTypes
	{
		TargetNone = 0x0,
		TargetMoveable = 0x1
	};

	//TargetEffect is an effect that is applied directly to a target object
	class TargetEffect : public Effect
	{
	protected:
		EffectTargetTypes validTargetTypes;

	public:
		TargetEffect(PhysicalWorld* parentWorld); 

		//ApplyEffectToTarget will be called on any object the effect should target.
		//This method must be overriden in sub-classes.  The sub-class's implementation
		//must decide what type of object to cast the target to (for example, a force effect
		//should cast the target parameter to a Moveable).
		virtual void ApplyEffectToTarget(void* target) = 0;

		bool IsTargetValid(EffectTargetTypes targetType) const {return (this->validTargetTypes & targetType) > 0;}
		EffectTargetTypes GetValidTargetTypes() const {return this->validTargetTypes;}
	};*/
}