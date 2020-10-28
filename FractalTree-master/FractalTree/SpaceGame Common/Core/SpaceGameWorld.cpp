#include "pch.h"
#include "SpaceGameWorld.h"

using namespace SpaceGameCore;
using namespace DirectX;

void SpaceGameWorld::Update()
{
	if (this->active)
	{
		this->applyPathing();
	}
	__super::Update();
}

void SpaceGameWorld::applyPathing()
{
	for (int a = 0; a < this->GetObjectCount(); ++a)
	{
		if ((this->objectList[a]->GetComponentFlags() & PhysicalWorld::PATH) > 0)
		{
			SimulationEngine::Actor* const actor = static_cast<SimulationEngine::Actor*>(this->objectList[a]);
			XMVECTOR loc = actor->location;
			XMFLOAT4 newLoc;
			XMStoreFloat4(&newLoc, loc);
			
			if (newLoc.y <= 0)
			{
				newLoc.y = 0;
				loc = XMLoadFloat4(&newLoc);
				actor->location = loc;
				actor->resting = true;
				const XMVECTOR drag = XMVectorMultiply(actor->velocity, XMVectorReplicate(actor->mass * -.1f));
				actor->ApplyForceAtPoint(actor->location, drag);
			}
			else
			{
				actor->resting = false;
			}
		}
	}
}