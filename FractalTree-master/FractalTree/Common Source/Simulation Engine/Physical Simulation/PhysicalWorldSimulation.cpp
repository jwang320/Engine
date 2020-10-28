#include "pch.h"
#include "PhysicalWorldSimulation.h"

using namespace DirectX;

namespace SimulationEngine
{
	PhysicalWorldSimulation::PhysicalWorldSimulation() :
		playerCount(0),
		nextAvailableGlobalPlayerID(0),
		localPlayerID(-1)
	{

	}

	void PhysicalWorldSimulation::ResetWorld()
	{
		if(this->world != NULL)
		{
			delete this->world;
		}

		this->world = new PhysicalWorld();
	}

	void PhysicalWorldSimulation::Initialize()
	{
		__super::Initialize();
		this->playerIDMap.clear();
		this->playerIDMap.resize(10000, -1);
		this->ResetWorld();
	}

	//Returns the global ID of the newly created player.
	int PhysicalWorldSimulation::CreatePlayer(int playerID)
	{
		Player* newPlayer = new Player();
		if(this->playerList.size() < this->playerCount)
		{
			this->playerList[this->playerCount] = newPlayer;
		}
		else
		{
			this->playerList.push_back(newPlayer);
		}

		this->playerIDMap[playerID] = this->playerCount;
		newPlayer->SetGlobalID(playerID);
		++this->playerCount;
		this->playerMap[playerID] = newPlayer;
		this->InitializePlayer(newPlayer);

		return newPlayer->GetGlobalID();
	}

	int PhysicalWorldSimulation::CreatePlayer(Player* newPlayer, int playerID)
	{
		if (this->playerList.size() < this->playerCount)
		{
			this->playerList[this->playerCount] = newPlayer;
		}
		else
		{
			this->playerList.push_back(newPlayer);
		}

		this->playerIDMap[playerID] = this->playerCount;
		this->playerMap[playerID] = newPlayer;
		newPlayer->SetGlobalID(playerID);
		++this->playerCount;

		this->InitializePlayer(newPlayer);

		return newPlayer->GetGlobalID();
	}

	XMVECTOR PhysicalWorldSimulation::GetCameraLocation(int playerID) const
	{
		if(this->PlayerExists(playerID) && this->GetWorld()->IsObjectAlive(this->GetPlayer(playerID)->GetFocusObject()))
		{
			return this->GetActor(this->GetPlayer(playerID)->GetFocusObject())->location;
		}

		return XMVectorSet(0, 150, 200, 0);
	}

	XMVECTOR PhysicalWorldSimulation::GetCameraFacing(int playerID) const
	{
		if(this->PlayerExists(playerID) && this->IsObjectValid(this->GetPlayer(playerID)->GetFocusObject()))
		{
			return this->GetActorFacing(this->GetPlayer(playerID)->GetFocusObject());
			//return static_cast<Actor*>(this->GetWorldObjectByGlobalID(this->GetPlayer(playerID)->GetFocusObject()))->GetLocalZ();
		}

		return XMVectorSet(0, -150, -200, 0);
	}

	XMVECTOR PhysicalWorldSimulation::GetCameraOrientation(int playerID) const
	{
		if(this->PlayerExists(playerID) && this->IsObjectValid(this->GetPlayer(playerID)->GetFocusObject()))
		{
			return this->GetActorViewDirection(this->GetPlayer(playerID)->GetFocusObject());
		}

		return XMVectorSet(0, 0, 0, 0);
	}

	XMVECTOR PhysicalWorldSimulation::GetCameraUp(int playerID) const
	{
		if(this->PlayerExists(playerID) && this->IsObjectValid(this->GetPlayer(playerID)->GetFocusObject()))
		{
			const XMMATRIX rot = XMMatrixRotationQuaternion(this->GetActor(this->GetPlayer(playerID)->GetFocusObject())->qOrientation);
			//return static_cast<Actor*>(this->GetWorldObjectByGlobalID(this->GetPlayer(playerID)->GetFocusObject()))->GetLocalY();
			return XMVector3Transform(XMVectorSet(0, 1, 0, 0), rot);
		}

		return XMVectorSet(0, 1, 0, 0);
	}
	void PhysicalWorldSimulation::SendCommandPlayer(int playerID, const Command& command)
	{
		if (this->PlayerExists(playerID))
		{
			const int focusId = this->GetPlayer(playerID)->GetFocusObject();
			if (this->IsObjectValid(focusId)) {
				this->SendCommand(this->GetPlayer(playerID)->GetFocusObject(), command, playerID);
			}
		}
	}


	void PhysicalWorldSimulation::SendCommand(int objectID, const Command& command, int playerId)
	{
		if (this->IsObjectValid(objectID))
		{
			Actor* const obj = this->GetActor(objectID);
			if (obj->canPlayerInteract(playerId)) {
				obj->ReceiveCommand(command);
			}
		}
	}

	//Overload this function to specify automatic initialization for created players.
	void PhysicalWorldSimulation::InitializePlayer(Player* player)
	{

	}

	PhysicalWorldSimulation::~PhysicalWorldSimulation()
	{

	}
}