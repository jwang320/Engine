#include "pch.h"
#include "PlatformerPersonCharacter.h"
#include "BlockUniverse.h"
#include "PlatformerMissile.h"
#include <fstream>
#include "GameResources.h"

using namespace SimulationEngine;
using namespace DirectX;

PlatformerPersonCharacter::PlatformerPersonCharacter(BlockUniverse* parentBlockUniverse, int owningCharacter, int owningPlayer, const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
	PlatformerCharacter(parentBlockUniverse, false, owningCharacter, owningPlayer, objectParameters, moveableParameters),
	gunFacingAngle(0),
	mouseX(0),
	mouseY(0)
{
	this->objectType = ObjectTypes::PlatformerPersonCharacter;
	this->movementFriction = true;

	Thruster* leftThruster = new Thruster();
	Thruster* rightThruster = new Thruster();

	leftThruster->direction = XMVectorSet(-1, 0, 0, 0);
	rightThruster->direction = XMVectorSet(1, 0, 0, 0);

	leftThruster->requireResting = false;
	rightThruster->requireResting = false;

	this->AddThruster(leftThruster);
	this->AddThruster(rightThruster);

	const float speed = .25f;
	this->thrusters[0]->currentMagnitude = speed * this->mass;
	this->thrusters[1]->currentMagnitude = speed * this->mass;

	this->thrusters[0]->airControlFactor = 0;// .25f;
	this->thrusters[1]->airControlFactor = 0;// .25f;

	this->blockList.clear();
	this->blockList.shrink_to_fit();
	this->canWalkStairs = true;
	this->loadBlockModel(GameResources::PlayerShipType);
	this->useDiscreteAnimation = GameResources::UseDiscreteAnimationPlayer;
	/*this->AddBlockAnimation(this->blockList.size() - 1, 30, 1, 0);
	this->AddBlockAnimation(this->blockList.size() - 1, 60, 1, 0);
	this->AddBlockAnimation(this->blockList.size() - 1, 90, -1, 0);
	this->AddBlockAnimation(this->blockList.size() - 1, 120, -1, 0);*/
}

void PlatformerPersonCharacter::ReceiveCommand(const Command& command)
{
	switch (command.command)
	{
	case StrafeLeft:
		if (command.flags & 1)
		{
			this->thrusters[0]->activated = true;
		}
		else
		{
			this->thrusters[0]->activated = false;
		}
		break;
	case StrafeRight:
		if (command.flags & 1)
		{
			this->thrusters[1]->activated = true;
		}
		else
		{
			this->thrusters[1]->activated = false;
		}
	
		break;
	case Look:
	{
		const XMVECTOR facingVec = XMVector3Transform(XMVectorSet(1, 0, 0, 0), XMMatrixRotationZ(this->gunFacingAngle));
		const XMVECTOR cP = XMVector3Cross(facingVec, XMVectorSet(command.arg1, command.arg2, 0, 0));
		XMFLOAT4 ans;
		XMStoreFloat4(&ans, cP);
		this->gunFacingAngle -= ans.z;
	}
		break;
	case Jump:
		if (this->resting)
		{
			this->ApplyForceAtPoint(this->location, XMVectorSet(0, 8.25f * this->mass, 0, 0));
			this->SetRested(false);
		}
		break;
	case Shoot:
		this->FireGuns(this->gunFacingAngle);
		break;
	case Shoot2:
		this->die();
		break;
	}
}