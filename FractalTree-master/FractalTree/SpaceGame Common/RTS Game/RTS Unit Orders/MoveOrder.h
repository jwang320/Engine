#pragma once
#include "..\..\RTS Core\RTSUnitOrder.h"

namespace RTSGame
{
	class MoveOrder : public RTSCore::RTSUnitOrder
	{
	protected:
		const DirectX::XMVECTOR destination;

	public:
		MoveOrder(RTSCore::SpaceGameWorld* const gameWorld, unsigned int owningUnitId, float xDestination, float zDestination);
		virtual void Initialize() override;
		virtual bool CheckCompletionCondition() override;
		virtual void Complete() override;

		static const int TypeId = 1;
	};
}