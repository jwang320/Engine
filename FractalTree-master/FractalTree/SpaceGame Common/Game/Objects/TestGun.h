#pragma once
#include "../Common Source/Simulation Engine/Physical Simulation/Item.h"

namespace SpaceGameCore
{
	class TestGun : public SimulationEngine::Item
	{
	public:
		TestGun(SimulationEngine::Actor* owner);
		virtual void use() override;
	};
}

