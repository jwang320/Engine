#include "pch.h"
#include "IneffectiveMissileLauncher.h"
#include "IneffectiveMissile.h"

using namespace DirectX;

namespace SimulationEngine
{
	IneffectiveMissileLauncher::IneffectiveMissileLauncher(Actor* owner) :
		Weapon(owner)
	{

	}

	void IneffectiveMissileLauncher::Shoot()
	{
		const XMVECTOR newLoc = XMVectorAdd(this->owner->GetFacing(), XMVectorAdd(this->owner->location, XMVectorSet(0, 24, 0, 0)));
		IneffectiveMissile* p = new IneffectiveMissile(ObjectParameters(this->owner->GetParentWorld()),
			MoveableParameters(newLoc,
			//this->owner->GetViewDirection(),
			XMVectorAdd(XMVectorSet(-2.1415f *.5f, 0, 0, 0), this->owner->qOrientation),
			XMVectorSet(0, 0, 0, 0),
			XMVectorSet(0, 0, 0, 0)));

		//p->ApplyForceAtPoint(p->GetLocation(), XMVectorMultiply(this->owner->GetLocalZ(), XMVectorReplicate(4000.25f)));
		//p->SetVelocity(XMVectorMultiply(this->owner->GetFacing(), XMVectorReplicate(1.f)));
	}
}