#include "pch.h"
#include "ConstraintD.h"

using namespace DirectX;
using namespace MathUtil;

namespace SimulationEngine
{
	ConstraintD::ConstraintD(Actor* const object1, Actor* const object2) :
		object1(object1),
		object2(object2)
	{
	}
	ConstraintD::ConstraintD(Actor* const object1, const XMVECTOR& constraintPoint) :
		object1(object1),
		object2(nullptr),
		constraintPoint(fromDirectX(constraintPoint))
	{
	}
	ConstraintD::ConstraintD(Actor* const object1, const XMVECTOR& constraintPoint, const XMVECTOR& offset1) :
		object1(object1),
		object2(nullptr),
		constraintPoint(fromDirectX(constraintPoint)),
		offset1(offset1)
	{
	}
	ConstraintD::ConstraintD(Actor* const object1,
		Actor* const object2,
		const XMVECTOR& offset1,
		const XMVECTOR& offset2,
		const bool immovableObject2) :
		object1(object1),
		object2(object2),
		offset1(offset1),
		offset2(offset2),
		immovableObject2(immovableObject2)
	{
	}

	void ConstraintD::UpdateFrame()
	{
		using namespace Eigen;
		this->leverArm1 = this->GetLeverArm1();
		this->vel1 = fromDirectX(this->object1->velocity);
		this->angVel1 = fromDirectX(this->object1->pitchYawRollVelocity);

		const Vector3d transformedPoint1 = fromDirectX(this->object1->location) + this->leverArm1;
		Vector3d transformedPoint2;
		if (this->object2 != nullptr)
		{
			this->vel2 = fromDirectX(this->object2->velocity);
			this->angVel2 = fromDirectX(this->object2->pitchYawRollVelocity);
			this->leverArm2 = this->GetLeverArm2();
			transformedPoint2 = fromDirectX(this->object2->location) + this->leverArm2;
		}
		else
		{
			this->leverArm2 = Vector3d(0, 0, 0);
			transformedPoint2 = this->constraintPoint;
		}


		//correction term for constraint drift; only applies when constraint is violated
		this->velocityBias = fromDirectX(XMVectorMultiply(XMVectorSubtract(fromEigen(transformedPoint2), fromEigen(transformedPoint1)), XMVectorReplicate(-ResourceManager::angleBB / (1.0f / ResourceManager::timeStepCount))));
		
		const Matrix3d skew1 = skewSymmetric(this->leverArm1);
		const Matrix3d skew1T = skew1.transpose();
		const Matrix3d mass1 = Eigen::DiagonalMatrix<double, 3>(1.0/object1->mass, 1.0/object1->mass, 1.0/object1->mass);
		const Matrix3d iA = mass1;
		
		if (this->object2 != nullptr && !immovableObject2)
		{

			//object 1 and object 2 are both movable
			const Matrix3d skew2 = skewSymmetric(this->leverArm2);
			const Matrix3d skew2T = skew2.transpose();
			const Matrix3d mass2 = Eigen::DiagonalMatrix<double, 3>(1.0/object2->mass, 1.0/object2->mass, 1.0/object2->mass);
			const Matrix3d iB = mass2;

			const Matrix3d A = mass1 + mass2 + (skew1 * iA * skew1T) + (skew2 * iB * skew2T);
			this->invMass = A.inverse();

			/*if (FractalTreeResources::constraintUseWarmStart)
			{
				this->object2->SetVelocity(XMVectorAdd(this->object2->GetVelocity(), this->accumulatedImpulse));
				this->object2->SetPitchYawRollVelocity(XMVectorAdd(this->object2->GetPitchYawRollVelocity(),
					XMVector3Cross(this->leverArm2, this->accumulatedImpulse)));
			}*/
		}
		else
		{
			//object 1 connected to world or immovable object
			const Matrix3d A = mass1 + (skew1 * iA * skew1T);
			this->invMass = A.inverse();
		}

		/*if (FractalTreeResources::constraintUseWarmStart)
		{
			this->object1->SetVelocity(XMVectorSubtract(this->object1->GetVelocity(), this->accumulatedImpulseD));
			this->object1->SetPitchYawRollVelocity(XMVectorSubtract(this->object1->GetPitchYawRollVelocity(),
				XMVector3Cross(this->leverArm1, this->accumulatedImpulseD)));
		}*/

		this->accumulatedImpulse = Vector3d(0, 0, 0);
	}
}
