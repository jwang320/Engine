#pragma once
#include "IConstraint.h"

extern const float BB;

namespace SimulationEngine
{
	//static const TestMat oneMatrix = TestMat(DirectX::XMVectorSet(1, 1, 1, 0),
	//	DirectX::XMVectorSet(1, 1, 1, 0), DirectX::XMVectorSet(1, 1, 1, 0));
	static const TestMat oneMatrix = TestMat(DirectX::XMVectorSet(1, 0, 0, 0),
		DirectX::XMVectorSet(0, 1, 0, 0), DirectX::XMVectorSet(0, 0, 1, 0));
	static const TestMat massMatrix = TestMat(DirectX::XMVectorSet(1, 0, 0, 0),
		DirectX::XMVectorSet(0, 1, 0, 0), DirectX::XMVectorSet(0, 0, 1, 0));

	class Actor;

	class Constraint : public IConstraint
	{
	public:
		Actor* const object1;
		Actor* const object2; //null if connected to world instead of object
		const DirectX::XMVECTOR constraintPoint = DirectX::XMVectorZero();
		const DirectX::XMVECTOR offset1 = DirectX::XMVectorZero();
		const DirectX::XMVECTOR offset2 = DirectX::XMVectorZero();
		const DirectX::XMVECTOR originalConstraintDistance;
		const bool immovableObject2 = false;
		DirectX::XMVECTOR transformedPoint1;
		DirectX::XMVECTOR transformedPoint2;
		DirectX::XMVECTOR currentConstraintDistance;
		DirectX::XMVECTOR leverArm1;
		DirectX::XMVECTOR leverArm2;

	public:
		Constraint(Actor* const object1, Actor* const object2); //two objects
		Constraint(Actor* const object1, const DirectX::XMVECTOR& constraintPoint); //connected to world
		Constraint(Actor* const object1, const DirectX::XMVECTOR& constraintPoint, const DirectX::XMVECTOR& offset1); //connected to world
		Constraint(Actor* const object1, Actor* const object2, const DirectX::XMVECTOR& offset1, const DirectX::XMVECTOR& offset2, const bool immovableObject2 = false);

		DirectX::XMVECTOR computeError() const;
		inline DirectX::XMVECTOR GetConstraintPoint1() const
		{
			using namespace DirectX;
			return XMVectorAdd(XMVector3Transform(this->offset1, XMMatrixRotationQuaternion(this->object1->qOrientation)),
				this->object1->location);
		}

		inline DirectX::XMVECTOR GetConstraintPoint2() const
		{
			using namespace DirectX;
			return this->object2 == nullptr ? this->constraintPoint : XMVectorAdd(XMVector3Transform(this->offset2, XMMatrixRotationQuaternion(this->object2->qOrientation)),
				this->object2->location);
		}

		inline DirectX::XMVECTOR GetCurrentDistance() const
		{
			using namespace DirectX;
			return XMVectorSubtract(this->GetConstraintPoint2(), this->GetConstraintPoint1());
		}

		inline DirectX::XMVECTOR GetLeverArm1() const
		{
			using namespace DirectX;
			return XMVector3Transform(this->offset1, XMMatrixRotationQuaternion(this->object1->qOrientation));
		}

		inline DirectX::XMVECTOR GetLeverArm2() const
		{
			using namespace DirectX;
			return XMVector3Transform(this->offset2, XMMatrixRotationQuaternion(this->object2->qOrientation));
		}

		void UpdateFrame();
	};
}