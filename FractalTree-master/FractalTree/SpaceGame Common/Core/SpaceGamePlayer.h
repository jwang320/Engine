#pragma once
#include <unordered_set>
#include "..\Common Source\Simulation Engine\Physical Simulation\Player.h"


namespace SpaceGameCore
{
	class SpaceGameSimulation;

	class SpaceGamePlayer : public SimulationEngine::Player
	{
	public:
		SpaceGameSimulation* rtsSimulation;
	public:
		SpaceGamePlayer(SpaceGameSimulation* simulation, const unsigned int newPlayerId = 0);
	};
}
