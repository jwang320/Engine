#include "pch.h"
#include "SpaceBall.h"

using namespace SpaceGameCore;
using namespace SimulationEngine;
using namespace DirectX;

SpaceBall::SpaceBall(const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
	SimulationEngine::Actor(objectParameters, moveableParameters)
{

	float thrusterMag = 330.25f;
	float turnMag = .1;

	this->objectType = ObjectTypes::SpaceBall;
	this->SetScale(XMVectorSet(5, 5, 5, 1));
	Thruster* const backLeftThruster = new Thruster();
	this->AddThruster(backLeftThruster);
	backLeftThruster->requireResting = false;
	backLeftThruster->direction = XMVectorSet(0, 0, 1, 0);
	backLeftThruster->offset = XMVectorSet(-10, 0, -1, 1);
	backLeftThruster->maximumMagnitude = turnMag;
	backLeftThruster->currentMagnitude = 0;
	backLeftThruster->airControlFactor = 1;

	Thruster* const backRightThruster = new Thruster();
	this->AddThruster(backRightThruster);
	backRightThruster->requireResting = false;
	backRightThruster->direction = XMVectorSet(0, 0, 1, 0);
	backRightThruster->offset = XMVectorSet(10, 0, -1, 1);
	backRightThruster->maximumMagnitude = turnMag;
	backRightThruster->currentMagnitude = 0;
	backRightThruster->airControlFactor = 1;
	this->componentFlags |= PhysicalWorld::PHYSICS;
	this->componentFlags |= PhysicalWorld::ACT;
	this->ignoreForceEffects = true;
}

void SpaceGameCore::SpaceBall::Act()
{
	if (++this->frameCount % 10 == 0)
	{
		thrusterState = rand() % 4;

		if (thrusterState == 1)
		{
			this->thrusters[0]->activated = true;
			this->thrusters[0]->currentMagnitude = .1;
			//this->FireThrusterPercent(this->thrusters[0], 10, 1);
		}
		else
		{
			this->thrusters[0]->activated = false;
		}
	}

	__super::Act();
}
