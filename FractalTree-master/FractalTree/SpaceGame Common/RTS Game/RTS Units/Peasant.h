#pragma once
#include "..\..\RTS Core\RTSUnit.h"

namespace RTSGame
{
	class Peasant : public RTSCore::RTSUnit
	{
	public:
		Peasant(int owningPlayer, const SimulationEngine::ObjectParameters& objectParameters = SimulationEngine::ObjectParameters(),
			const SimulationEngine::MoveableParameters& moveableParameters = SimulationEngine::MoveableParameters());
	};
}