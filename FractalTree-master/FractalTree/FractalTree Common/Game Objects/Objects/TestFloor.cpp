#include "pch.h"
#include "TestFloor.h"

using namespace DirectX;

namespace SimulationEngine
{
	TestFloor::TestFloor(const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
		Actor(objectParameters, moveableParameters)
	{
		//this->scale = XMVectorSet(4096, 1, 4096, 0);
		this->objectType = ObjectTypes::TestFloor;
	}

}