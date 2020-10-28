#pragma once
#include <unordered_set>
#include "..\Common Source\Simulation Engine\Physical Simulation\Player.h"


namespace RTSCore
{
	class RTSUnit;
	class SpaceGameSimulation;

	class RTSPlayer : public SimulationEngine::Player
	{
	public:
		float xCoord = 0;
		float zCoord = 0;

		float xCoordVel = 0;
		float zCoordVel = 0;
		std::vector<int> playerUnitIds;
		std::unordered_set<int> selectedUnitsIds;
		SpaceGameSimulation* rtsSimulation;
		float mouseX = 0;
		float mouseY = 0;
		float scrollVel = .25f;
	public:
		RTSPlayer(SpaceGameSimulation* simulation, const unsigned int newPlayerId = 0);
		void SelectUnit(int globalUnitId);
		//will deselect unit if already selected
		void ReverseSelection(int globalUnitId);
	};
}
