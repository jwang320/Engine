#pragma once
#include "IConstraintD.h"

namespace SimulationEngine
{
	class Actor;
	//a constraint which enforces the angle between two objects at their connection remain constant
	//maybe a motor can be made by varying the angle constant over time
	class AngleConstraintD : public IConstraintD
	{
	public:
		Actor* const object1;
		Actor* const object2; //null if connected to world instead of object
		const DirectX::XMVECTOR constraintPoint = DirectX::XMVectorSet(0, 0, 0, 0);
		const DirectX::XMVECTOR offset1 = DirectX::XMVectorSet(0, 0, 0, 0);
		const DirectX::XMVECTOR offset2 = DirectX::XMVectorSet(0, 0, 0, 0);
		const bool immovableObject2 = false;
		Eigen::Vector3d leverArm1;
		Eigen::Vector3d leverArm2;

	public:
		AngleConstraintD(Actor* const object1, Actor* const object2); //two objects
		AngleConstraintD(Actor* const object1, const DirectX::XMVECTOR& constraintPoint); //connected to world
		AngleConstraintD(Actor* const object1, const DirectX::XMVECTOR& constraintPoint, const DirectX::XMVECTOR& offset1); //connected to world
		AngleConstraintD(Actor* const object1, Actor* const object2, const DirectX::XMVECTOR& offset1, const DirectX::XMVECTOR& offset2, const bool immovableObject2 = false);

		inline Eigen::Vector3d GetLeverArm1() const
		{
			using namespace DirectX;
			return MathUtil::fromDirectX(XMVector3Transform(this->offset1, XMMatrixRotationQuaternion(this->object1->qOrientation)));
			//return fromDirectX(this->offset1).transpose() * (qFromD(this->object1->qOrientation).toRotationMatrix());
		}

		inline Eigen::Vector3d GetLeverArm2() const
		{
			using namespace DirectX;
			return MathUtil::fromDirectX(XMVector3Transform(this->offset2, XMMatrixRotationQuaternion(this->object2->qOrientation)));
		}

		void UpdateFrame();
	};
}