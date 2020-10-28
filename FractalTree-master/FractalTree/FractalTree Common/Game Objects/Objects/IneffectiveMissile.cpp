#include "pch.h"
#include "IneffectiveMissile.h"
#include "..\Common Source\Simulation Engine\Physical Simulation\PhysicalWorld.h"
#include "..\Common Source\Simulation Engine\Physical Simulation\Effects\ForceEffect.h"

using namespace DirectX;

namespace SimulationEngine
{
	IneffectiveMissile::IneffectiveMissile(const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
		Actor(objectParameters, moveableParameters),
		age(0)
	{
		this->mass = 10;
		this->componentFlags |= PhysicalWorld::PHYSICS;
		this->componentFlags |= PhysicalWorld::ACT;
		this->objectType = ObjectTypes::IneffectiveMissile;

		this->scale = XMVectorSet(4, 4, 4, 1);
		Thruster* upThruster = new Thruster();

		upThruster->direction = XMVectorSet(0, 1, 0, 0);
		upThruster->maximumMagnitude = .17881f * this->mass;
		upThruster->currentMagnitude = .17881f * this->mass;
		upThruster->activated = true;
		upThruster->requireResting = false;
		//this->AddThruster(upThruster);

		Thruster* forwardThruster = new Thruster();

		forwardThruster->direction = XMVectorSet(0, 0, 1, 0);
		forwardThruster->maximumMagnitude = .421955f * this->mass;
		forwardThruster->currentMagnitude = .421955f * this->mass;
		forwardThruster->activated = true;
		forwardThruster->requireResting = false;
		forwardThruster->offset = XMVectorSet(0, 0, 20, 0);
		//this->AddThruster(forwardThruster);

		this->pitchYawRollVelocity = XMVectorSet(.0215f, .1415f, 0, 0);
		//this->qAngularVelocity = XMQuaternionRotationRollPitchYawFromVector(XMVectorSet(.0215f, .1415f, 0, 0));
	}

	void IneffectiveMissile::Act()
	{
		__super::Act();
		if(this->age < 2000005)
		{
			++this->age;
		}
	}

	void IneffectiveMissile::OnCollision(const CollisionData& collisionData)
	{
		if(this->age > 200)
		{
			if(rand()% 3 == -1)
				return;
			ForceEffect* explosionEffect = new ForceEffect(EffectParameters(static_cast<PhysicalWorld*>(this->parentWorld), true), ForceEffectParameters(this->location, XMVectorSet(0, 0, 0, 0), 18500000, false, false, false, 1800*1800, .1f));

			this->Remove();
		}
		else
		{
			//this->ApplyForceAtPoint(XMVectorAdd(this->location, XMVector3Normalize(this->facing)), collisionData.collisionNormal);
		}
	}
}