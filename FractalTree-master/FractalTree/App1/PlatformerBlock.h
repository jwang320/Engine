#include "PlatformerCharacter.h"

class PlatformerBlock : public PlatformerCharacter
{

public:
	PlatformerBlock(BlockUniverse* parentBlockUniverse, int owningCharacter = -1, int owningPlayer = 0, const SimulationEngine::ObjectParameters& objectParameters = SimulationEngine::ObjectParameters(),
		const SimulationEngine::MoveableParameters& moveableParameters = SimulationEngine::MoveableParameters());
};