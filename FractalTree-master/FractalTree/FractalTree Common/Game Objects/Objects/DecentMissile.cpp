#include "pch.h"
#include "DecentMissile.h"
#include "..\Common Source\Simulation Engine\Physical Simulation\PhysicalWorld.h"
#include "..\Common Source\Simulation Engine\Physical Simulation\Effects\ForceEffect.h"

using namespace DirectX;

namespace SimulationEngine
{
	DecentMissile::DecentMissile(const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters, const bool killImmediately, const float force) :
		Actor(objectParameters, moveableParameters),
		remainingFrames(10),
		dying(false),
		killImmediately(killImmediately),
		force(force)
	{
		this->mass = 10;
		this->componentFlags |= PhysicalWorld::PHYSICS;
		this->componentFlags |= PhysicalWorld::ACT;
		this->objectType = ObjectTypes::DecentMissile;

#ifndef _WIN8_1
		Thruster* upThruster = new Thruster();

		upThruster->direction = XMVectorSet(0, 1, 0, 0);
		upThruster->maximumMagnitude = .17881f * this->mass;
		upThruster->currentMagnitude = .17881f * this->mass;
		upThruster->activated = true;
		upThruster->requireResting = false;
		if(!ResourceManager::useShip)
		this->AddThruster(upThruster);
#endif
		/*Thruster* forwardThruster = new Thruster();

		forwardThruster->direction = XMVectorSet(0, 0, 1, 0);
		forwardThruster->maximumMagnitude = .21955f * this->mass;
		forwardThruster->currentMagnitude = .21955f * this->mass;
		forwardThruster->activated = true;
		forwardThruster->requireResting = false;
		forwardThruster->offset = XMVectorSet(0, 0, 20, 0);

		this->dragCoefficient = 0;//.005f;
		//this->AddThruster(forwardThruster);*/
	}

	void DecentMissile::Act()
	{
		__super::Act();
		if (this->killImmediately)
		{
			this->Remove();
		}
		/*if(this->dying)
		{
			--this->remainingFrames;
			if(this->remainingFrames < 0)
			{
				this->Remove();
			}
		}*/
	}

	void DecentMissile::OnCollision(const CollisionData& collisionData)
	{
		if(rand()% 3 == 1)
		{
			//this->pitchYawRollVelocity = XMVectorSet(0, .05f, 0, 0);
			//return;
		}
		ForceEffect* explosionEffect = new ForceEffect(EffectParameters(static_cast<PhysicalWorld*>(this->parentWorld), true), ForceEffectParameters(this->location, XMVectorSet(0, 0, 0, 0), force, false, false, false, 400*400, .1f));
		/*explosionEffect->SetInstant(true);
		this->dying = true;
		this->componentFlags ^= PhysicalWorld::PHYSICS;
		this->componentFlags ^= PhysicalWorld::MOVE;
		this->velocity = XMVectorSet(0, 0, 0, 0);*/
		this->Remove();
	}
}