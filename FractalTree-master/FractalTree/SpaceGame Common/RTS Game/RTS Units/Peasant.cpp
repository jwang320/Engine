#include "pch.h"
#include "Peasant.h"
#include "..\RTS Unit Orders\MoveOrder.h"
#include "..\Common Source\Simulation Engine\Physical Simulation\PhysicalWorld.h"

using namespace SimulationEngine;
using namespace DirectX;
using namespace RTSCore;

namespace RTSGame
{
	Peasant::Peasant(int owningPlayer, const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
		RTSUnit(owningPlayer, objectParameters, moveableParameters)
	{
		this->objectType = ObjectTypes::Peasant;


		this->collisionType = NONE;
		

		this->facing = XMVectorSet(0, 0, 10, 0);
		this->qOrientation = XMQuaternionIdentity();
		

		this->componentFlags |= PhysicalWorld::PHYSICS;
		this->componentFlags |= PhysicalWorld::PATH;

		this->availableOrders.insert(MoveOrder::TypeId);
	}
}