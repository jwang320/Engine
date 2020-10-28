#include "pch.h"
#include "AxisConstraint.h"

using namespace DirectX;

namespace SimulationEngine
{
	AxisConstraint::AxisConstraint(Actor* const object1, Actor* const object2) :
		object1(object1),
		object2(object2),
		originalConstraintDistance(XMVectorSubtract(object2->location, this->GetConstraintPoint1()))
	{
	}
	AxisConstraint::AxisConstraint(Actor* const object1, const XMVECTOR& constraintPoint) :
		object1(object1),
		object2(nullptr),
		constraintPoint(constraintPoint),
		originalConstraintDistance(XMVectorSubtract(constraintPoint, this->GetConstraintPoint1()))
	{
	}
	AxisConstraint::AxisConstraint(Actor* const object1, const XMVECTOR& constraintPoint, const XMVECTOR& offset1) :
		object1(object1),
		object2(nullptr),
		constraintPoint(constraintPoint),
		offset1(offset1),
		originalConstraintDistance(XMVectorSubtract(constraintPoint, this->GetConstraintPoint1()))
	{

	}
	AxisConstraint::AxisConstraint(Actor* const object1,
		Actor* const object2,
		const XMVECTOR& offset1,
		const XMVECTOR& offset2,
		const bool immovableObject2) :
		object1(object1),
		object2(object2),
		offset1(offset1),
		offset2(offset2),
		originalConstraintDistance(XMVectorSubtract(this->GetConstraintPoint2(), this->GetConstraintPoint1())),
		immovableObject2(immovableObject2)
	{
	}
}
