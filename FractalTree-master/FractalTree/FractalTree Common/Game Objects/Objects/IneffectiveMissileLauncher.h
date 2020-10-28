#pragma once
#include "Weapon.h"

namespace SimulationEngine
{
	class IneffectiveMissileLauncher : public Weapon
	{
	public:
		IneffectiveMissileLauncher(Actor* owner = NULL);

	protected:
		virtual void Shoot() override;
	};
}