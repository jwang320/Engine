#pragma once
#include "../Common Source/Simulation Engine/Physical Simulation/Item.h"

namespace SimulationEngine
{
	class Weapon : public Item
	{
	protected:
		long long lastUseTime;
		long long coolDown;

	public:
		void ShootWeapon();

	protected:
		Weapon(Actor* owner = NULL, long long coolDown = 100);
		virtual void Shoot() = 0;
	};
}