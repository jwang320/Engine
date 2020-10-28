#include "pch.h"
#include "MoveOrder.h"
#include "..\..\RTS Core\RTSUnit.h"

using namespace RTSCore;
using namespace DirectX;

namespace RTSGame
{
	MoveOrder::MoveOrder(SpaceGameWorld* const gameWorld, unsigned int owningUnitId, float xDestination, float zDestination) :
		RTSUnitOrder(gameWorld, owningUnitId, MoveOrder::TypeId),
		destination(XMVectorSet(xDestination, 0, zDestination, 0))
	{
		this->getOwner()->SetMoveDestination(destination);
	}

	void MoveOrder::Initialize()
	{
		this->getOwner()->SetMoveState(MoveState::MOVE);
	}

	bool MoveOrder::CheckCompletionCondition()
	{
		const RTSUnit* const unit = this->getOwner();
		const XMVECTOR currentLocation = unit->location;
		XMFLOAT4 dist;
		XMStoreFloat4(&dist, XMVector3Length(XMVectorSubtract(currentLocation, destination)));
		return dist.x < .1f;
	}

	void MoveOrder::Complete()
	{
		this->getOwner()->SetMoveState(MoveState::STOP);
	}
}
