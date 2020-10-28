#include "pch.h"
#include "RTSUnitOrder.h"
#include "SpaceGameWorld.h"

namespace RTSCore
{
	RTSUnitOrder::RTSUnitOrder(SpaceGameWorld* const gameWorld, unsigned int owningUnitId, int orderType) :
		gameWorld(gameWorld),
		owningUnitId(owningUnitId),
		owningUnit(gameWorld->GetUnit(owningUnitId)),
		orderType(orderType)
	{
	}

	RTSUnit* RTSUnitOrder::getOwner()
	{
		return this->gameWorld->GetUnit(this->owningUnitId);
	}
}
