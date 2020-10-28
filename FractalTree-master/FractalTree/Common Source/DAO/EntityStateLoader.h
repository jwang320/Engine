#pragma once
#include <vector>

struct EntityState
{
	SimulationEngine::ObjectParameters objectParamers;
	SimulationEngine::ActorParameters actorParamers;
	SimulationEngine::MoveableParameters moveableParameters;
	ObjectTypes objectType;
	bool persist;
};

class EntityStateLoader
{
protected:
	bool initialized = false;

public:
	virtual unsigned int getObjectId(const unsigned int owner, const unsigned int objectType, const bool persist) = 0;
	virtual std::vector<EntityState> getPlayerEntityStates(const unsigned int owner) = 0;
	virtual void savePlayerEntityStates(const std::vector<EntityState>& entityStates, const unsigned int owner) = 0;
	bool isInitialize() const { return this->initialized; }
	virtual ~EntityStateLoader() {}
};