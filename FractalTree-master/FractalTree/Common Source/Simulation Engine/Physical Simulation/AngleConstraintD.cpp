#include "pch.h"
#include "AngleConstraintD.h"
#include "UIRenderer.h"

using namespace DirectX;
using namespace MathUtil;

namespace SimulationEngine
{
	AngleConstraintD::AngleConstraintD(Actor* const object1, Actor* const object2) :
		object1(object1),
		object2(object2)
	{
	}
	AngleConstraintD::AngleConstraintD(Actor* const object1, const XMVECTOR& constraintPoint) :
		object1(object1),
		object2(nullptr),
		constraintPoint(constraintPoint)
	{
	}
	AngleConstraintD::AngleConstraintD(Actor* const object1, const XMVECTOR& constraintPoint, const XMVECTOR& offset1) :
		object1(object1),
		object2(nullptr),
		constraintPoint(constraintPoint),
		offset1(offset1)
	{
	}
	AngleConstraintD::AngleConstraintD(Actor* const object1,
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

	void AngleConstraintD::UpdateFrame()
	{
		using namespace Eigen;
		using namespace DirectX;
		this->leverArm1 = this->GetLeverArm1();
		const Vector3d transformedPoint1 = fromDirectX(this->object1->location) = this->leverArm1;

		this->leverArm2 = this->GetLeverArm2();
		const Vector3d transformedPoint2 = fromDirectX(this->object2->location) + this->leverArm2;

		const Matrix3d skew1 = skewSymmetric(this->leverArm1);
		const Matrix3d skew1T = skew1.transpose();
		const Matrix3d mass1 = Eigen::DiagonalMatrix<double, 3>(object1->mass, object1->mass, object1->mass);
		const Matrix3d iA = mass1;

		if (this->object2 != nullptr && !immovableObject2)
		{

			//object 1 and object 2 are both movable
			const Matrix3d skew2 = skewSymmetric(this->leverArm2);
			const Matrix3d skew2T = skew2.transpose();
			const Matrix3d mass2 = Eigen::DiagonalMatrix<double, 3>(object2->mass, object2->mass, object2->mass);
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

		const Eigen::Quaterniond targetQuaternaion = qFromD(this->object1->constraintTargetTest);
		const Eigen::Quaterniond quaternionBetweenObjects = qFromD(this->object1->qOrientation) * qFromD(this->object2->qOrientation).inverse();
		const Eigen::Quaterniond quaternionError = targetQuaternaion * quaternionBetweenObjects.inverse();

		const Eigen::AngleAxisd errorAxis = Eigen::AngleAxisd(quaternionError);
		
		const double step = -ResourceManager::angleBB / (1.0 / ResourceManager::timeStepCount);
		Vector3d axis = errorAxis.axis();
		axis.normalize();
		const double angle = errorAxis.angle();
		const Vector3d targetAngularVelocity = axis.cwiseProduct(Vector3d(angle, angle, angle));
		const Vector3d finalTargetBias = targetAngularVelocity.cwiseProduct(Vector3d(step, step, step));

		this->velocityBias = finalTargetBias.transpose() * qFromD(this->object1->qOrientation).toRotationMatrix();

		/*if (FractalTreeResources::constraintUseWarmStart)
		{
			this->object1->SetVelocity(XMVectorSubtract(this->object1->GetVelocity(), this->accumulatedImpulse));
			this->object1->SetPitchYawRollVelocity(XMVectorSubtract(this->object1->GetPitchYawRollVelocity(),
				XMVector3Cross(this->leverArm1, this->accumulatedImpulse)));
		}*/

		this->accumulatedImpulse = Vector3d(0, 0, 0);
	}
}
