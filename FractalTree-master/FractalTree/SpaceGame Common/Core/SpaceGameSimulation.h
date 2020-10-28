#pragma once
#include "SpaceGamePlayer.h"
#include <vector>
#include "..\Common Source\Simulation Engine\Physical Simulation\PhysicalWorldSimulation.h"

class EntityStateLoader;

namespace SpaceGameCore
{
	//1 byte
#define COMMAND_TYPE_POSITION 0
	//2 bytes
#define OBJECT_COMMANDS_TYPE_POSITION COMMAND_TYPE_POSITION + 1
	//global id of commanded object (4 bytes)
#define OBJECT_COMMANDS_TARGET_ID_POSITION OBJECT_COMMANDS_TYPE_POSITION + 1

	//for commands with a single position destination
	//x position of move location destination (4 bytes)
#define OBJECT_COMMANDS_TARGET_LOCATION_X_POSITION OBJECT_COMMANDS_TARGET_ID_POSITION + 4
	//z position of move location destination (4 bytes)
#define OBJECT_COMMANDS_TARGET_LOCATION_Z_POSITION OBJECT_COMMANDS_TARGET_LOCATION_X_POSITION + 4
	//y position of move location destination (4 bytes)
#define OBJECT_COMMANDS_TARGET_LOCATION_Y_POSITION OBJECT_COMMANDS_TARGET_LOCATION_Z_POSITION + 4

	enum class CommandType : unsigned char
	{
		NONE,
		OBJECT_COMMAND
	};

	enum class ObjectCommands : unsigned short
	{
		NONE,
		MOVE
	};

	class SpaceGamePlayer;

	class SpaceGameSimulation : public SimulationEngine::PhysicalWorldSimulation
	{
	private:
		EntityStateLoader* idProvider = nullptr;

	public:
		SpaceGameSimulation();
		void InitializeWorld();
		virtual void Initialize() override;
		void ResetWorld();
		void InitializeLocalPlayer(int playerID);
		virtual void Update() override;
		void ClearWorld();
		void EnterWorld();
		SpaceGamePlayer* getPlayer(int id) { return static_cast<SpaceGamePlayer*>(this->playerMap.at(id)); }
		const SpaceGamePlayer* getPlayer(int id) const { return static_cast<const SpaceGamePlayer*>(this->playerMap.at(id)); }
		void InitializeNewPlayer(const unsigned int newPlayerId);
		void UpdateObjectFromSerializedData(const char* const data);
		void ReceiveCommand(unsigned int playerId, const char* data);
		void SetObjectIdProvider(EntityStateLoader* const idProvider) { this->idProvider = idProvider; }
		void joinPlayer(const unsigned int playerId);
		void leavePlayer(const unsigned int playerId);
	};
}