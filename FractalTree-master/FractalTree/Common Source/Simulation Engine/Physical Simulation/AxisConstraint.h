#pragma once

namespace SimulationEngine
{
	class Actor;
	//a constraint which enforces the angle between two objects at their connection remain constant
	//maybe a motor can be made by varying the angle constant over time
	class AxisConstraint
	{
	public:
		Actor * const object1;
		Actor* const object2; //null if connected to world instead of object
		const DirectX::XMVECTOR constraintPoint = DirectX::XMVectorSet(0, 0, 0, 0);
		const DirectX::XMVECTOR offset1 = DirectX::XMVectorSet(0, 0, 0, 0);
		const DirectX::XMVECTOR offset2 = DirectX::XMVectorSet(0, 0, 0, 0);
		const DirectX::XMVECTOR originalConstraintDistance;
		const bool immovableObject2 = false;
		mutable DirectX::XMVECTOR lastImpulse = DirectX::XMVectorZero();
		mutable DirectX::XMVECTOR transformedPoint1;
		mutable DirectX::XMVECTOR transformedPoint2;
		mutable DirectX::XMVECTOR currentConstraintDistance;
		mutable DirectX::XMVECTOR leverArm1;
		mutable DirectX::XMVECTOR leverArm2;
		mutable DirectX::XMVECTOR normalizedLeverArm1;
		mutable DirectX::XMVECTOR normalizedLeverArm2;
		mutable DirectX::XMVECTOR velocityBias;
		mutable TestMat invMass;
		static int count;

	public:
		AxisConstraint(Actor* const object1, Actor* const object2); //two objects
		AxisConstraint(Actor* const object1, const DirectX::XMVECTOR& constraintPoint); //connected to world
		AxisConstraint(Actor* const object1, const DirectX::XMVECTOR& constraintPoint, const DirectX::XMVECTOR& offset1); //connected to world
		AxisConstraint(Actor* const object1, Actor* const object2, const DirectX::XMVECTOR& offset1, const DirectX::XMVECTOR& offset2, const bool immovableObject2 = false);

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

		void UpdateFrame() const;
	};
}