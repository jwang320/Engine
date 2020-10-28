#include "pch.h"
#include "Weapon.h"
#include <Windows.h>
extern double performanceFrequency;

namespace SimulationEngine
{
	Weapon::Weapon(Actor* owner, long long coolDown) :
		Item(owner),
		coolDown(coolDown),
		lastUseTime(0)
	{
		
	}

	void Weapon::ShootWeapon()
	{
		_LARGE_INTEGER currentTime; QueryPerformanceCounter(&currentTime);
		if((currentTime.QuadPart - this->lastUseTime) / performanceFrequency > this->coolDown)
		{
			this->Shoot();
			this->lastUseTime = currentTime.QuadPart;
		}	
	}
}