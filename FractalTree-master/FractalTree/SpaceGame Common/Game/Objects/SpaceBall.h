#pragma once
#include "../Common Source/Simulation Engine/Physical Simulation/Actor.h"

namespace SpaceGameCore
{
	class SpaceBall : public SimulationEngine::Actor
	{
	private:
		unsigned long long frameCount = 0;
		int thrusterState = 0;
	public:
		SpaceBall(const SimulationEngine::ObjectParameters& objectParameters = SimulationEngine::ObjectParameters(),
			const SimulationEngine::MoveableParameters& moveableParameters = SimulationEngine::MoveableParameters());

		virtual void Act() override;

	};

}

