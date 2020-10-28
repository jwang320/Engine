#pragma once
#include "IConstraintD.h"

namespace SimulationEngine
{
	class Actor;

	class ConstraintD : public IConstraintD
	{
	public:
		Actor* const object1;
		Actor* const object2; //null if connected to world instead of object
		const Eigen::Vector3d constraintPoint = Eigen::Vector3d(0, 0, 0);
		const DirectX::XMVECTOR offset1 = DirectX::XMVectorZero();
		const DirectX::XMVECTOR offset2 = DirectX::XMVectorZero();
		const bool immovableObject2 = false;
		Eigen::Vector3d leverArm1;
		Eigen::Vector3d leverArm2;
		Eigen::Vector3d vel1;
		Eigen::Vector3d vel2;
		Eigen::Vector3d angVel1;
		Eigen::Vector3d angVel2;

	public:
		ConstraintD(Actor* const object1, Actor* const object2); //two objects
		ConstraintD(Actor* const object1, const DirectX::XMVECTOR& constraintPoint); //connected to world
		ConstraintD(Actor* const object1, const DirectX::XMVECTOR& constraintPoint, const DirectX::XMVECTOR& offset1); //connected to world
		ConstraintD(Actor* const object1, Actor* const object2, const DirectX::XMVECTOR& offset1, const DirectX::XMVECTOR& offset2, const bool immovableObject2 = false);

		inline Eigen::Vector3d GetConstraintPoint1() const
		{
			using namespace DirectX;
			return MathUtil::fromDirectX(XMVectorAdd(XMVector3Transform(this->offset1, XMMatrixRotationQuaternion(this->object1->qOrientation)),
				this->object1->location));
		}

		inline Eigen::Vector3d GetConstraintPoint2() const
		{
			using namespace DirectX;
			return this->object2 == nullptr ? this->constraintPoint : MathUtil::fromDirectX(XMVectorAdd(XMVector3Transform(this->offset2, XMMatrixRotationQuaternion(this->object2->qOrientation)),
				this->object2->location));
		}

		inline Eigen::Vector3d GetLeverArm1() const
		{
			using namespace DirectX;
			return MathUtil::fromDirectX(XMVector3Transform(this->offset1, XMMatrixRotationQuaternion(this->object1->qOrientation)));
		}

		inline Eigen::Vector3d GetLeverArm2() const
		{
			using namespace DirectX;
			return MathUtil::fromDirectX(XMVector3Transform(this->offset2, XMMatrixRotationQuaternion(this->object2->qOrientation)));
		}

		void UpdateFrame();
	};
}