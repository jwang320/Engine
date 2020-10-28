#include "pch.h"
#include "SpaceGameSimulation.h"
#include "SpaceGameWorld.h"
#include "SpaceGamePlayer.h"
#include "../Game/Objects/TestFloor.h"
#include "../Game/Objects/Humanoid.h"
#include "../Game/Objects/SpaceBall.h"
#include "../Common Source/Simulation Engine/Base Simulation/Object.h"
#include <DirectXMath.h>
#include "../Common Source/DAO/EntityStateLoader.h"

using namespace SimulationEngine;
using namespace DirectX;
using namespace SpaceGameCore;

SpaceGameSimulation::SpaceGameSimulation()
{

}

void SpaceGameSimulation::InitializeWorld()
{
	auto a = new TestFloor(ObjectParameters(this->GetWorld()));
	a->location = XMVectorSet(5, 0, 0, 1);
	ForceEffect* const gravityEffect = new ForceEffect(EffectParameters(this->GetWorld()), ForceEffectParameters(XMVectorSet(0, 0, 0, 0), XMVectorSet(0, -1, 0, 0), .181f));

	//new SpaceBall(ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(0, 50, 30, 0)));
}

void SpaceGameSimulation::Initialize()
{
	__super::Initialize();

}

void SpaceGameSimulation::ResetWorld()
{
	if (this->world != NULL)
	{
		delete this->world;
	}

	this->world = new SpaceGameWorld();
}

void SpaceGameSimulation::InitializeLocalPlayer(int playerID)
{
	if (playerID < 0)
	{
		SpaceGamePlayer* const player = new SpaceGamePlayer(this);
		this->GetPlayer(this->CreatePlayer(player, this->nextAvailableGlobalPlayerID));
		++this->nextAvailableGlobalPlayerID;
		this->localPlayerID = 0;
	}
	else
	{
		this->localPlayerID = playerID;
		SpaceGamePlayer* const player = new SpaceGamePlayer(this);
		this->GetPlayer(this->CreatePlayer(player, playerID));
	}
}

void SpaceGameSimulation::Update()
{
	__super::Update();
}

void SpaceGameSimulation::ClearWorld()
{
	if (this->PlayerExists(0))
	{
		if (this->playerList[0]->GetFocusObject() == this->world->GetObjectGlobalID(this->world->GetObjectCount() - 1))
		{
			return;
		}
	}

	this->world->RemoveObject(this->world->GetObjectGlobalID(this->world->GetObjectCount() - 1));
}

void SpaceGameSimulation::EnterWorld()
{
	this->InitializeLocalPlayer(-1);
	Humanoid * testHumanoid = new Humanoid(ObjectParameters(this->GetWorld()));
	this->playerList[0]->SetFocusObject(testHumanoid->GetGlobalID());
}

void SpaceGameSimulation::InitializeNewPlayer(const unsigned int newPlayerId)
{
	if (false && this->idProvider != nullptr)
	{
		const unsigned int newId = this->idProvider->getObjectId(newPlayerId, static_cast<int>(ObjectTypes::Humanoid), false);
	}
	else {
		SpaceGamePlayer* const newPlayer = new SpaceGamePlayer(this, newPlayerId);
		this->playerMap[newPlayerId] = newPlayer;
		ObjectParameters params(this->GetWorld());
		Humanoid* const p = new Humanoid(params, MoveableParameters(XMVectorSet(0, 0, 0, 1), XMQuaternionIdentity(), XMVectorSet(0, 0, .1f, 0)));
		newPlayer->SetFocusObject(p->GetGlobalID());
		p->setOwningPlayer(newPlayerId);
	}
}

void SpaceGameSimulation::joinPlayer(const unsigned int playerId)
{
	SpaceGamePlayer* const newPlayer = new SpaceGamePlayer(this, playerId);
	this->playerMap[playerId] = newPlayer;

	if (this->idProvider != nullptr)
	{
		std::vector<EntityState> playerEntityStates = this->idProvider->getPlayerEntityStates(playerId);
		for (EntityState& entityState : playerEntityStates)
		{
			Actor* newActor;
			entityState.objectParamers.parentWorld = this->world;
			switch (entityState.objectType)
			{
			case ObjectTypes::Humanoid:
				newActor = new Humanoid(entityState.objectParamers, entityState.moveableParameters);
				break;
			}

			newActor->setOwningPlayer(playerId);
		}
	}
}

void SpaceGameSimulation::leavePlayer(const unsigned int playerId)
{
	std::vector<EntityState> playerEntityStates;
	for (int i = 0; i < this->world->GetObjectCount(); ++i)
	{
		Actor* o = static_cast<Actor*>(this->world->objectList[i]);
		if (o->GetOwningPlayer() == playerId)
		{
			EntityState e;
			e.objectParamers.globalID = o->GetGlobalID();
			e.objectParamers.persistentId = o->persistentId;
			e.moveableParameters.location = o->location;
			e.objectType = o->GetObjectType();
			playerEntityStates.push_back(e);

			if (!o->persist)
			{
				o->Remove();
			}	
		}
	}

	if (this->idProvider != nullptr)
	{
		this->idProvider->savePlayerEntityStates(playerEntityStates, playerId);
	}
}

void SpaceGameSimulation::UpdateObjectFromSerializedData(const char* const data)
{
	const const int const const const const const id = Object::GetSerializedObjectId(data);
	if (this->DoesObjectExist(id))
	{
		XMVECTOR location, velocity, facing, scale, orientation;
		int owningPlayer;
		Actor::DeserializeObjectProperties(data + Object::SerializedObjectLength, location, scale, orientation, facing, velocity, owningPlayer);
		Actor* const updateObject = static_cast<Actor*>(this->GetWorldObjectByGlobalID(id));
		QueryPerformanceCounter(&updateObject->lastUpdated);
		updateObject->location = location;
		updateObject->velocity = velocity;
		updateObject->SetScale(scale);
		if (updateObject->GetObjectType() != ObjectTypes::Humanoid) {
			updateObject->SetFacing(facing);
			updateObject->SetPitchYawRoll(orientation);
		}
	}
	else if (!this->GetWorld()->IsObjectInitializeQueued(id))
	{
		const ObjectTypes objectType = Object::GetSerializedObjectType(data);
		XMVECTOR location, velocity, facing, scale, orientation;
		int owningPlayer;
		Actor::DeserializeObjectProperties(data + Object::SerializedObjectLength, location, scale, orientation, facing, velocity, owningPlayer);
		Actor* a = nullptr;
		switch (objectType)
		{
		case ObjectTypes::TestFloor:
		{
			TestFloor* const p = new TestFloor(ObjectParameters(this->world, -1, id), MoveableParameters(location, orientation, velocity, XMVectorSet(0, 0, 0, 0)));
			p->setOwningPlayer(owningPlayer);
			a = p;
		}
		break;
		case ObjectTypes::Humanoid:
		{
			Humanoid* const p = new Humanoid(ObjectParameters(this->world, -1, id), MoveableParameters(location, orientation, velocity, XMVectorSet(0, 0, 0, 0)));
			p->setOwningPlayer(owningPlayer);
			if (p->GetOwningPlayer() == this->localPlayerID) {
				this->GetPlayer(this->localPlayerID)->SetFocusObject(p->GetGlobalID());
			}
			a = p;
		}
		break;
		case ObjectTypes::SpaceBall:
		{
			SpaceBall* const p = new SpaceBall(ObjectParameters(this->world, -1, id), MoveableParameters(location, orientation, velocity, XMVectorSet(0, 0, 0, 0)));
			p->setOwningPlayer(owningPlayer);
			a = p;
		}
		default:
			break;
		}

		if (a != nullptr)
		{
			QueryPerformanceCounter(&a->lastUpdated);
		}
	}
}
void SpaceGameSimulation::ReceiveCommand(unsigned int playerId, const char* const data)
{
	const Command command = SimulationEngine::Command::deserialize(data);
	this->SendCommand(command.objectId, command, playerId);
}
