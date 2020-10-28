#pragma once
#include "PlatformerCharacter.h"

//I didn't spell Mesquito incorrectly.  It's Mesquite.  Like the wood.

class PlatformerMesquito : public PlatformerCharacter
{
public:
	PlatformerMesquito(BlockUniverse* parentBlockUniverse, int owningCharacter = -1, int owningPlayer = 0, const SimulationEngine::ObjectParameters& objectParameters = SimulationEngine::ObjectParameters(),
		const SimulationEngine::MoveableParameters& moveableParameters = SimulationEngine::MoveableParameters(), PlatformerCharacter* player = 0);

protected:
	PlatformerCharacter* player;
	int playerID;
};