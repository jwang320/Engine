#pragma once
#include "..\Common Source\Simulation Engine\Physical Simulation\PhysicalWorld.h"

namespace SpaceGameCore
{

	class SpaceGameWorld : public SimulationEngine::PhysicalWorld
	{
	public:
		
	protected:
		virtual void Update() override;

	private:
		void applyPathing();
	};
}