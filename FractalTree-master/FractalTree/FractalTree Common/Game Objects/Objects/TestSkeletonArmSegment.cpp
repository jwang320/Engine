#include "pch.h"
#include "TestSkeletonArmSegment.h"

using namespace SimulationEngine;

TestSkeletonArmSegment::TestSkeletonArmSegment(const ObjectParameters& objectParameters,
	const MoveableParameters& moveableParameters) :
	Actor(objectParameters, moveableParameters)
{
	this->objectType = ObjectTypes::TestSkeletonArmSegment;
}
