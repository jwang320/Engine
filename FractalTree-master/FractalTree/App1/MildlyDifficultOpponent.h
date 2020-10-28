#pragma once
#include "PlatformerCharacter.h"

class MildlyDifficultOpponent : public PlatformerCharacter
{
public:
	MildlyDifficultOpponent(BlockUniverse* parentBlockUniverse, int owningCharacter = -1, int owningPlayer = 0, const SimulationEngine::ObjectParameters& objectParameters = SimulationEngine::ObjectParameters(),
		const SimulationEngine::MoveableParameters& moveableParameters = SimulationEngine::MoveableParameters(), PlatformerCharacter* player = 0);

protected:
	PlatformerCharacter* player;
	int playerID;
	int coolDown;
};