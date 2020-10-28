#pragma once

namespace SimulationEngine
{
	class TestFloor : public Actor
	{
	public:
		TestFloor(const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters());
	};
}