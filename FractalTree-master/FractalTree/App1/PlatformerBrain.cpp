#include "pch.h"
#include "PlatformerBrain.h"
#include "PlatformerCharacter.h"

using namespace SimulationEngine;
using namespace DirectX;
using std::vector;

PlatformerBrain::PlatformerBrain(PlatformerCharacter* body, const AIBrainParameters& parameters) :
	body(body),
	currentState(SEARCH),
	engageMinDistance(parameters.engageMinDistance),
	searchRadius(parameters.searchRadius),
	searchHeight(parameters.searchHeight),
	weaponFireMaxDistance(parameters.weaponFireMaxDistance),
	targetObjectID(-1),
	outOfRangeAction(parameters.outOfRangeAction)
{

}

void PlatformerBrain::Think()
{
	switch (currentState)
	{
	case IDLE:
		this->BeIdle();
		break;
	case FOLLOW:
		this->ExecuteFollow();
		break;
	case SEARCH:
		this->ExecuteSearch();
		break;
	case ENGAGE:
		this->ExecuteEngage();
		break;
	}

	this->body->brain.Update();
}

//Idle at target location
void PlatformerBrain::BeIdle()
{

}

void PlatformerBrain::ExecuteFollow()
{
	if (!this->body->parentWorld->IsObjectAlive(this->targetObjectID))
	{
		this->SetCurrentState(SEARCH);
		return;
	}

	const PlatformerCharacter* target = static_cast<PlatformerCharacter*>(body->parentWorld->GetObjectByGlobalID(this->targetObjectID));
	const float thrustMag = 1;
	XMFLOAT4 distFromSearchSpot;
	XMStoreFloat4(&distFromSearchSpot, XMVectorSubtract(target->GetLocation(), this->body->location));

	if (fabs(distFromSearchSpot.x) > this->engageMinDistance)
	{
		if (distFromSearchSpot.x > 0)
		{
			this->body->brain.motorCortex.AddCommand(0, { 1, this->body->thrusters[0], thrustMag });
		}
		else
		{
			this->body->brain.motorCortex.AddCommand(0, { 1, this->body->thrusters[1], thrustMag });
		}
	}
	else
	{
		this->SetCurrentState(ENGAGE);
	}

	XMFLOAT4 bodyLoc;
	XMStoreFloat4(&bodyLoc, this->body->location);
	XMFLOAT4 targetLoc;
	XMStoreFloat4(&targetLoc, target->GetLocation());
	if (bodyLoc.y < targetLoc.y)
	{
		this->body->brain.motorCortex.AddCommand(0, { 5, this->body->thrusters[2], thrustMag });
	}
}

void PlatformerBrain::ExecuteSearch()
{
	ObjectTypes exclusionList[3] = { ObjectTypes::BlockUniverse, ObjectTypes::PlatformerMissile, ObjectTypes::PlatformerBlock };
	vector<Actor*> targetList;
	static_cast<PhysicalWorld*>(this->body->parentWorld)->QueryObjectsLocation(targetList, body->location, this->engageMinDistance * this->engageMinDistance, exclusionList, 3, body->owningPlayer);
	if (targetList.size() > 0)
	{
		this->targetObjectID = targetList[0]->GetGlobalID();
		this->SetCurrentState(ENGAGE);
	}
	else
	{
		const float thrustMag = 1;
		XMFLOAT4 distFromSearchSpot;
		XMStoreFloat4(&distFromSearchSpot, XMVectorSubtract(this->targetLocation, this->body->location));
		if (fabs(distFromSearchSpot.x) > this->searchRadius)
		{
			if (distFromSearchSpot.x > 0)
			{
				this->body->brain.motorCortex.AddCommand(0, { 1, this->body->thrusters[0], thrustMag });
			}
			else
			{
				this->body->brain.motorCortex.AddCommand(0, { 1, this->body->thrusters[1], thrustMag });
			}
		}
		else
		{
			if (rand() % 2 == 0)
			{
				this->body->brain.motorCortex.AddCommand(0, { 1, this->body->thrusters[0], thrustMag });
			}
			else
			{
				this->body->brain.motorCortex.AddCommand(0, { 1, this->body->thrusters[1], thrustMag });
			}
		}

		XMFLOAT4 bodyLoc;
		XMStoreFloat4(&bodyLoc, this->body->location);
		if (bodyLoc.y < this->searchHeight)
		{
			this->body->brain.motorCortex.AddCommand(0, { 5, this->body->thrusters[2], thrustMag });
		}
	}
}

void PlatformerBrain::ExecuteEngage()
{
	if (!this->body->parentWorld->IsObjectAlive(this->targetObjectID))
	{
		this->SetCurrentState(SEARCH);
		return;
	}

	if (this->body->weaponFrameCount == 0)
	{
		const PlatformerCharacter* target = static_cast<PlatformerCharacter*>(body->parentWorld->GetObjectByGlobalID(this->targetObjectID));
		XMFLOAT4 playerDist;
		XMStoreFloat4(&playerDist, XMVector3Length(XMVectorSubtract(this->body->location, target->GetLocation())));
		if (playerDist.x < this->weaponFireMaxDistance)
		{
			this->body->FireGuns(target->GetLocation());
		}
		else
		{
			this->SetCurrentState(SEARCH);

			switch (this->outOfRangeAction)
			{
			case RETURN:
				break;
			case STAY:
				this->targetLocation = this->body->location;
				break;
			case CHASE:
				this->SetCurrentState(FOLLOW);
				break;
			}
		}
	}
}

void PlatformerBrain::SetCurrentState(State newState)
{
	this->currentState = newState;
}