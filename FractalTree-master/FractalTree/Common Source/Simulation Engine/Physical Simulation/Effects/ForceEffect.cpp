#include "pch.h"
#include "ForceEffect.h"
#include "..\PhysicalWorld.h"

using namespace DirectX;

namespace SimulationEngine
{
	const SpatialEffectParameters ForceEffect::forceEffectSpatialParameters = {PhysicalWorld::PHYSICS};

	ForceEffect::ForceEffect(const EffectParameters& effectParameters, const ForceEffectParameters& forceEffectParameters) :
		SpatialEffect(effectParameters, forceEffectSpatialParameters),
		magnitude(forceEffectParameters.magnitude),
		location(forceEffectParameters.location),
		forceVector(XMVectorMultiply(forceEffectParameters.direction, XMVectorReplicate(forceEffectParameters.magnitude))),
		ignoreMass(forceEffectParameters.ignoreMass),
		relativeLocation(forceEffectParameters.relativeLocation),
		filterResting(forceEffectParameters.filterResting),
		maxRadius(forceEffectParameters.maxRadius),
		distanceFactor(forceEffectParameters.distanceFactor)
	{

	}

	void ForceEffect::ApplyEffectToTarget(Actor* target)
	{	
		if(this->parentWorld->IsObjectRegistered(target->GetGlobalID(), this->validTargetTypes) && !target->IgnoreForceEffects())
		{
			if(!(this->filterResting && target->resting))
			{
				//Compensate for the mass of the object if ignoring mass
				if(this->ignoreMass)
				{
					//Apply the force relative to the location of the target.
					if(this->relativeLocation)
					{
						target->ApplyForceAtPoint(XMVectorAdd(target->location, this->location), XMVectorMultiply(this->forceVector, XMVectorReplicate(target->mass)));
					}
					else //Apply the force at an absolute location
					{
						target->ApplyForceAtPoint(this->location, XMVectorMultiply(this->forceVector, XMVectorReplicate(target->mass)));
					}
				}
				else
				{
					XMFLOAT4 dist;
					const XMVECTOR len = XMVectorSubtract(target->location, this->location);
					XMStoreFloat4(&dist, XMVector3LengthSq(len));

					if(this->maxRadius > 0)
					{
						if(dist.x > maxRadius*maxRadius)
						{
							return;
						}
					}

					const XMVECTOR appliedForce = XMVectorMultiply(XMVector3Normalize(len), XMVectorReplicate(this->magnitude / max(1, (dist.x * this->distanceFactor))));
					target->ApplyForceAtPoint(target->location, appliedForce);
				}		
			}
		}
	}
}