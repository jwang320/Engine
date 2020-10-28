#pragma once
#include "PhysicalWorld.h"
#include "Player.h"

namespace SimulationEngine
{
	class PhysicalWorldSimulation : public Simulation
	{
	private:
		int playerCount;

	protected:
		std::vector<Player*> playerList;
		std::vector<int> playerIDMap;
		int nextAvailableGlobalPlayerID;
		std::unordered_map<unsigned int, Player*> playerMap;
		int localPlayerID;

	public:
		bool generateTerrain = false;
		PhysicalWorldSimulation();
		virtual void Initialize() override;
		void SendCommandPlayer(int playerID, const Command& command);
		void SendCommand(int objectID, const Command& command, int playerId);
		//Returns the global player ID of the newly created player.
		int CreatePlayer(int playerID);
		int CreatePlayer(Player* newPlayer, int playerID);
		bool PlayerExists(int globalID) const { return this->playerMap.find(globalID) != this->playerMap.end() || globalID >= 0 && this->playerIDMap[globalID] > -1; }
		~PhysicalWorldSimulation();

	protected:
		//Overload this function to determine what happens when a player is created.
		virtual void InitializePlayer(Player* player);
		virtual void ResetWorld();

	public:
#pragma region Getters and Setters
		Actor::ObjectData GetObjectData(int globalID) const {return this->GetWorld()->GetObjectData(globalID);}
		DirectX::XMVECTOR GetActorFacing(int globalID) const {return this->GetWorld()->GetActorFacing(globalID);}
		DirectX::XMVECTOR GetActorViewDirection(int globalID) const {return this->GetWorld()->GetActorViewDirection(globalID);}
		DirectX::XMVECTOR GetCameraLocation(int playerID) const;
		DirectX::XMVECTOR GetCameraFacing(int playerID) const;
		DirectX::XMVECTOR GetCameraOrientation(int playerID) const;
		DirectX::XMVECTOR GetCameraUp(int playerID) const;
		int GetLocalPlayerID() const {return this->localPlayerID;}
		void SetLocalPlayerID(int localPlayerID) {this->localPlayerID = localPlayerID;}
		bool IsObjectValid(int globalID) const {return this->GetWorld()->IsObjectAlive(globalID);}
		bool DoesObjectExist(int globalID) const {return this->GetWorld()->DoesObjetExist(globalID);}
		Player* GetPlayer(unsigned int globalID) const { return this->playerMap.at(globalID); }
		PhysicalWorld* GetWorld() const {return static_cast<PhysicalWorld*>(this->world);}
		Actor* GetActor(const int globalId) const { return this->GetWorld()->GetActor(globalId); }
	protected:
		int GetLocalPlayerID(int globalID) const { return this->playerIDMap[globalID]; }

#pragma endregion
	};
}