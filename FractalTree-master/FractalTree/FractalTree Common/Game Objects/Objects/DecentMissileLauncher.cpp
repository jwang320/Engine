#include "pch.h"
#include "DecentMissileLauncher.h"
#include "DecentMissile.h"

using namespace DirectX;

namespace SimulationEngine
{
	DecentMissileLauncher::DecentMissileLauncher(Actor* owner) :
		Weapon(owner)
	{

	}

	void DecentMissileLauncher::Shoot()
	{
		PhysicalWorld* const w = static_cast<PhysicalWorld*>(this->owner->GetParentWorld());
		XMVECTOR startLoc = XMVectorAdd(this->owner->location,
			XMVectorAdd(XMVectorSet(0, 35, 0, 0), XMVectorMultiply(this->owner->GetFacing(), XMVectorReplicate(5.f))));
		XMFLOAT4 shootLoc;
		XMFLOAT4 shootDir;
		XMStoreFloat4(&shootLoc, startLoc);
		XMStoreFloat4(&shootDir, this->owner->GetFacing());
		w->intersect(shootLoc.x, shootLoc.y, shootLoc.z, shootDir.x, shootDir.y, shootDir.z);
		//return;
		/*DecentMissile* p = new DecentMissile(ObjectParameters(this->owner->GetParentWorld()),
			MoveableParameters(XMVectorAdd(this->owner->GetLocation(), XMVectorSet(-4, 75, 0, 0)),
			this->owner->GetPitchYawRoll(),
			XMVectorSet(0, 0, 0, 0),
			XMVectorSet(0, 0, 0, 0)));*/
		
#ifndef _WIN8_1
		if(FractalTreeResources::useShip)
		{	
			DecentMissile* p = new DecentMissile(ObjectParameters(this->owner->GetParentWorld()),
			MoveableParameters(startLoc,
			this->owner->qOrientation,
			XMVectorSet(0, 0, 0, 0),
			XMVectorSet(0, 0, 0, 0)));
			p->SetVelocity(XMVectorAdd(/*this->owner->GetVelocity()*/XMVectorReplicate(0), XMVectorMultiply(this->owner->GetFacing(), XMVectorReplicate(50.f))));
		}
		else
		{
			//p->SetVelocity(XMVectorAdd(/*this->owner->GetVelocity()*/XMVectorReplicate(0), XMVectorMultiply(this->owner->GetFacing(), XMVectorReplicate(1.f))));
		}
#else
		p->SetVelocity(XMVectorAdd(/*this->owner->GetVelocity()*/XMVectorReplicate(0), XMVectorMultiply(this->owner->GetFacing(), XMVectorReplicate(1.f))));
#endif
	}
}