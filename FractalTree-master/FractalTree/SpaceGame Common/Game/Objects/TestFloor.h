#pragma once

namespace SpaceGameCore
{
	class TestFloor : public SimulationEngine::Actor
	{
	public:
		TestFloor(const SimulationEngine::ObjectParameters& objectParameters = SimulationEngine::ObjectParameters(),
			const SimulationEngine::MoveableParameters& moveableParameters = SimulationEngine::MoveableParameters());
	};
}