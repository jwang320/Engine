#pragma once
#include "Weapon.h"

namespace SimulationEngine
{
	class TestMissileLauncher : public Weapon
	{
	public:
		TestMissileLauncher(Actor* owner = NULL);

	protected:
		virtual void Shoot() override;
	};
}