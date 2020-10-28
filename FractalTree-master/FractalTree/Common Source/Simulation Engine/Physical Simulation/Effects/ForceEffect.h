#pragma once

#include "SpatialEffect.h"

namespace SimulationEngine
{
	enum class ForceEffectType : char{};
	struct ForceEffectParameters
	{
		const DirectX::XMVECTOR location;
		const DirectX::XMVECTOR direction;
		float magnitude;
		bool ignoreMass;
		bool relativeLocation;
		bool filterResting;
		float maxRadius;
		float distanceFactor;

		ForceEffectParameters(const DirectX::XMVECTOR& location = DirectX::XMVectorSet(0, 0, 0, 0), const DirectX::XMVECTOR& direction = DirectX::XMVectorSet(0, 0, 0, 0), float magnitude = 1, bool ignoreMass = true, bool relativeLocation = true, bool filterResting = true, float maxRadius = -1, float distanceFactor = 1) :
			location(location), direction(direction), magnitude(magnitude), ignoreMass(ignoreMass), relativeLocation(relativeLocation), filterResting(filterResting), maxRadius(maxRadius), distanceFactor(distanceFactor) {}
	};

	//ForceEffect applies a force at a given point to a target object.
	class ForceEffect : public SpatialEffect
	{
	protected:
		float magnitude;
		DirectX::XMVECTOR forceVector;
		DirectX::XMVECTOR location;
		//The force should be applied with the same acceleration regardless of an object's mass (example:  gravity)
		bool ignoreMass;
		//If true: the location of the applied force should relative to the target object.
		bool relativeLocation;
		bool filterResting;
		float maxRadius;
		float distanceFactor;

	private:
		static const SpatialEffectParameters forceEffectSpatialParameters;

	public:
		//The ignore mass field is to denote a force that applies the same acceleration regardless of mass (example:  gravity)
		ForceEffect(const EffectParameters& effectParameters, const ForceEffectParameters& forceEffectParameters = ForceEffectParameters());
		//This effect assumes the target extends Moveable
		virtual void ApplyEffectToTarget(Actor* target) override;
	};
}