#pragma once
#include "..\..\RTS Core\RTSUnit.h"

namespace RTSGame
{
	class Castle : public RTSCore::RTSUnit
	{
	public:
		Castle(int owningPlayer, const SimulationEngine::ObjectParameters& objectParameters = SimulationEngine::ObjectParameters(),
			const SimulationEngine::MoveableParameters& moveableParameters = SimulationEngine::MoveableParameters());
	};
}