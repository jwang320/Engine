#pragma once
#include "Weapon.h"

namespace SimulationEngine
{
	class DecentMissileLauncher : public Weapon
	{
	public:
		DecentMissileLauncher(Actor* owner = NULL);

	protected:
		virtual void Shoot() override;
	};
}