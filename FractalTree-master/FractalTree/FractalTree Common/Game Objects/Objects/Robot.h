#pragma once
#include "..\Common Source\Simulation Engine\Physical Simulation\Actor.h"
#include "..\Common Source\Simulation Engine\Base Simulation\Object.h"

namespace SimulationEngine
{
	class Robot : public Actor
	{
	public:
		Robot(const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters());

		virtual void Act() override;
	};
}