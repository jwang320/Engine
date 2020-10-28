#pragma once
#include "Simulation.h"
#include "World.h"

namespace SimulationEngine
{
	class World;

	class Simulation
	{
	protected:
		int simulationLevel = 0;

	public:
		World* world;
		bool paused = false;
		bool oneMoreFrame = false;

	public:
		virtual void Update();
		virtual void Initialize();
		virtual ~Simulation();

	protected:
		Simulation();

	public:
#pragma region Getters and Setters
		int GetObjectGlobalID(int localID) const { return this->world->GetObjectGlobalID(localID); }
		void ReleaseObject(int globalID) const { this->world->GetObjectByGlobalID(globalID)->Release(); }
		void LockObject(int globalID) const { this->world->GetObjectByGlobalID(globalID)->Lock(); }
		int GetObjectData(int globalID) const { return this->world->GetObjectData(globalID); }
		unsigned int GetObjectCount() const { return this->world->GetObjectCount(); }
		Object* GetWorldObjectByGlobalID(int globalID) const { return this->world->GetObjectByGlobalID(globalID); }
		World* GetSimulationWorld() const { return this->world; }
#pragma endregion
	};
}