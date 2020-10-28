#include "pch.h"
#include "Actor.h"
#include "PhysicalWorld.h"
#include "..\..\Skeleton\Skeleton.h"

using namespace DirectX;

namespace SimulationEngine
{
	Actor::Actor(const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters, const ActorParameters& actorParameters, int owningPlayer) :
		Object(objectParameters),
		Moveable(moveableParameters),
		facing(actorParameters.facing),
		owningPlayer(owningPlayer),
		world(static_cast<PhysicalWorld*>(objectParameters.parentWorld))
	{
		this->componentFlags |= PhysicalWorld::ACT;
		this->componentFlags |= PhysicalWorld::MOVE;
	}

	Actor::Actor(World* const parentWorld, const MoveableParameters& moveableParameters) :
		Object(ObjectParameters(parentWorld)),
		Moveable(moveableParameters),
		world(static_cast<PhysicalWorld*>(parentWorld))
	{
		this->componentFlags |= PhysicalWorld::ACT;
		this->componentFlags |= PhysicalWorld::MOVE;
	}

	void Actor::Act()
	{
		//this->brain.Update();
		/*if(!this->locked)
		{
			this->Move();
		}*/
	}

	void Actor::OnCollision(const CollisionData& collisionData)
	{

	}

 	void Actor::GetSerializedForm(char* data, int& dataLength, int maxLength) const
 	{
		if(Actor::SerializedActorLength > maxLength)
		{
			return;
		}

		Object::GetSerializedForm(data, dataLength, maxLength);
		XMFLOAT4 loc;
		XMStoreFloat4(&loc, this->location);
		/*memcpy(data + dataLength, &loc, sizeof(loc));
		dataLength+= sizeof(loc);*/
		memcpy(data + dataLength, &this->location, sizeof(this->location));
		dataLength+= sizeof(this->location);

		memcpy(data + dataLength, &this->scale, sizeof(this->scale));
		dataLength+= sizeof(this->scale);

		memcpy(data + dataLength, &this->qOrientation, sizeof(this->qOrientation));
		dataLength+= sizeof(this->qOrientation);

		memcpy(data + dataLength, &this->facing, sizeof(this->facing));
		dataLength+= sizeof(this->facing);

		memcpy(data + dataLength, &this->velocity, sizeof(this->velocity));
		dataLength += sizeof(this->velocity);

		memcpy(data + dataLength, &this->owningPlayer, sizeof(this->owningPlayer));
		dataLength += sizeof(this->owningPlayer);
 	}

	void Actor::GetSerializedFormSmall(char* data, int& dataLength, int maxLength) const
	{
		if(Actor::SerializedActorLengthSmall > maxLength)
		{
			return;
		}

		Object::GetSerializedFormSmall(data, dataLength, maxLength);

		memcpy(data + dataLength, &this->location, sizeof(this->location));
		dataLength+= sizeof(this->location);

		memcpy(data + dataLength, &this->scale, sizeof(this->scale));
		dataLength+= sizeof(this->scale);

		memcpy(data + dataLength, &this->qOrientation, sizeof(this->qOrientation));
		dataLength+= sizeof(this->qOrientation);

		memcpy(data + dataLength, &this->facing, sizeof(this->facing));
		dataLength+= sizeof(this->facing);

		memcpy(data + dataLength, &this->velocity, sizeof(this->velocity));
		dataLength+= sizeof(this->velocity);

		memcpy(data + dataLength, &this->owningPlayer, sizeof(this->owningPlayer));
		dataLength += sizeof(this->owningPlayer);
	}

	const int Actor::GetSerializedLength() const
	{
		return Object::GetSerializedLength();
	}

	void Actor::DeserializeObjectProperties(const char* data, XMVECTOR& location, XMVECTOR& scale, XMVECTOR& pitchYawRoll, XMVECTOR& facing, XMVECTOR& velocity, int& owningPlayer)
	{
		int currentIndex = 0;

		memcpy(&location, data, sizeof(location));
		currentIndex+= sizeof(location);

		memcpy(&scale, data + currentIndex, sizeof(scale));
		currentIndex+= sizeof(scale);

		memcpy(&pitchYawRoll, data + currentIndex, sizeof(pitchYawRoll));
		currentIndex+= sizeof(pitchYawRoll);

		memcpy(&facing, data + currentIndex, sizeof(facing));
		currentIndex+= sizeof(facing);

		memcpy(&velocity, data + currentIndex, sizeof(velocity));
		currentIndex += sizeof(velocity);

		memcpy(&owningPlayer, data + currentIndex, sizeof(owningPlayer));
		currentIndex += sizeof(owningPlayer);
	}

	Actor::~Actor()
	{

	}
}
