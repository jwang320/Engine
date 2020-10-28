#include "pch.h"
#include "TestMissileLauncher.h"
#include "Projectile.h"

using namespace DirectX;

namespace SimulationEngine
{
	TestMissileLauncher::TestMissileLauncher(Actor* owner) :
		Weapon(owner, 300)
	{

	}

	void TestMissileLauncher::Shoot()
	{
		{
			Projectile* p = new Projectile(ObjectParameters(this->owner->GetParentWorld()),
				MoveableParameters(XMVectorAdd(this->owner->location, XMVectorSet(0, 35, 0, 0)),
				this->owner->qOrientation,
				XMVectorSet(0, 0, 0, 0),
				XMVectorSet(0, 0, 0, 0)));
		
			//p->SetVelocity(XMVectorMultiply(this->owner->GetFacing(), XMVectorReplicate(1.f)));
			p->SetVelocity(XMVectorSet(0, 0, 1, 0));
		}

		{
			Projectile* p = new Projectile(ObjectParameters(this->owner->GetParentWorld()),
				MoveableParameters(XMVectorAdd(this->owner->location, XMVectorSet(0, 135, 0, 0)),
					this->owner->qOrientation,
					XMVectorSet(0, 0, 0, 0),
					XMVectorSet(0, 0, 0, 0)));

			//p->SetVelocity(XMVectorMultiply(this->owner->GetFacing(), XMVectorReplicate(1.f)));
			p->SetVelocity(XMVectorSet(1, 0, 0, 0));
		}

		
	}
}