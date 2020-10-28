#include "pch.h"
#include "Constraint.h"

using namespace DirectX;

namespace SimulationEngine
{
	Constraint::Constraint(Actor* const object1, Actor* const object2) :
		object1(object1),
		object2(object2),
		originalConstraintDistance(XMVectorSubtract(object2->location, this->GetConstraintPoint1()))
	{
	}
	Constraint::Constraint(Actor* const object1, const XMVECTOR& constraintPoint) :
		object1(object1),
		object2(nullptr),
		constraintPoint(constraintPoint),
		originalConstraintDistance(XMVectorSubtract(constraintPoint, this->GetConstraintPoint1()))
	{
	}
	Constraint::Constraint(Actor* const object1, const XMVECTOR& constraintPoint, const XMVECTOR& offset1) :
		object1(object1),
		object2(nullptr),
		constraintPoint(constraintPoint),
		offset1(offset1),
		originalConstraintDistance(XMVectorSubtract(constraintPoint, this->GetConstraintPoint1()))
	{
	}
	Constraint::Constraint(Actor* const object1,
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

	XMVECTOR Constraint::computeError() const
	{
		const XMVECTOR lA = this->GetLeverArm1();
		const XMVECTOR tP = XMVectorAdd(this->object1->location, lA);
		
		return XMVectorSubtract(this->constraintPoint, tP);
	}

	void Constraint::UpdateFrame()
	{
		this->leverArm1 = this->GetLeverArm1();
		this->transformedPoint1 = XMVectorAdd(this->object1->location, leverArm1);
		if (this->object2 != nullptr)
		{
			this->leverArm2 = this->GetLeverArm2();
			this->transformedPoint2 = XMVectorAdd(this->object2->location, leverArm2);
		}
		else
		{
			this->leverArm2 = XMVectorZero();
			this->transformedPoint2 = this->constraintPoint;
		}
		this->currentConstraintDistance = XMVectorSubtract(this->transformedPoint2, this->transformedPoint1);

		//correction term for constraint drift; only applies when constraint is violated
		this->velocityBias = XMVectorMultiply(this->currentConstraintDistance, XMVectorReplicate(-ResourceManager::angleBB / (1.0f / ResourceManager::timeStepCount)));
	
		const XMMATRIX skew1 = skewSymmetric(this->leverArm1);
		const XMMATRIX skew1T = XMMatrixTranspose(skew1);
		const XMMATRIX massA = this->object1->inverseMassMatrix;
		const XMMATRIX iA = MathUtil::diagonal(this->object1->rotationalInertia);
		//const TestMat skew1 = TestMat::b3SkewSymmetric(this->leverArm1);
		//const TestMat skew1T = TestMat::b3Transpose(skew1);
		//const TestMat r1 = TestMat::FromQuaternion(this->object1->qOrientation);
		//const TestMat iA = r1 * TestMat::b3Inverse(TestMat::diagonal(this->object1->GetRotationalInertia())) * TestMat::b3Transpose(r1);
		//const TestMat iA = TestMat::diagonal(this->object1->GetRotationalInertia());
		//const TestMat massA = TestMat::diagonal(this->object1->GetInvMass());
		
		if (this->object2 != nullptr && !immovableObject2)
		{
			//object 1 and object 2 are both movable
			/*const TestMat r2 = TestMat::FromQuaternion(this->object2->qOrientation);
			//const TestMat iB = r2 * TestMat::b3Inverse(TestMat::diagonal(this->object2->GetRotationalInertia())) * TestMat::b3Transpose(r2);
			const TestMat iB = TestMat::diagonal(this->object2->GetRotationalInertia());
			const TestMat massB = TestMat::diagonal(this->object2->GetInvMass());
			const TestMat skew2 = TestMat::b3SkewSymmetric(this->leverArm2);
			const TestMat skew2T = TestMat::b3Transpose(skew2);
			const TestMat A = massA + massB + (skew1 * iA * skew1T) + (skew2 * iB * skew2T);*/
			const XMMATRIX skew2 = skewSymmetric(this->leverArm2);
			const XMMATRIX skew2T = XMMatrixTranspose(skew2);
			const XMMATRIX massB = this->object2->inverseMassMatrix;
			const XMMATRIX iB = MathUtil::diagonal(this->object2->rotationalInertia);
			//const XMMATRIX A = MathUtil::add3(MathUtil::add3(MathUtil::add3(massA, massB), XMMatrixMultiply(skew1, XMMatrixMultiply(iA, skew1T))), XMMatrixMultiply(skew2, XMMatrixMultiply(iB, skew2T)));
			//const TestMat A = massA + massB + (skew1 * iA * skew1T) + (skew2 * iB * skew2T);
			const XMMATRIX A = MathUtil::add3(MathUtil::add3(massA, massB), MathUtil::add3(XMMatrixMultiply(XMMatrixMultiply(skew1, iA), skew1T), XMMatrixMultiply(XMMatrixMultiply(skew2, iB), skew2T)));
			/*XMVECTOR det;
			this->invMass = XMMatrixInverse(&det, A);
			XMFLOAT4 detF; XMStoreFloat4(&detF, det);
			if (fabs(detF.x) <= .000001f)
				this->invMass = XMMatrixIdentity();*/
				//this->invMass = MathUtil::xInverse(A);
			this->notInvMass = A;
			this->invMass = MathUtil::xInverse(A);

			if (ResourceManager::constraintUseWarmStart)
			{
				this->object2->SetVelocity(XMVectorAdd(this->object2->velocity, this->accumulatedImpulse));
				this->object2->SetPitchYawRollVelocity(XMVectorAdd(this->object2->pitchYawRollVelocity,
					XMVector3Cross(this->leverArm2, this->accumulatedImpulse)));
			}
		}
		else
		{
			//object 1 connected to world or immovable object
			//const TestMat A = massA + (skew1 * iA * skew1T);
			//const XMMATRIX A = MathUtil::add3(massA, XMMatrixMultiply(skew1, XMMatrixMultiply(iA, skew1T)));
			const XMMATRIX A = MathUtil::add3(massA, XMMatrixMultiply(XMMatrixMultiply(skew1, iA), skew1T));
			//this->invMass = TestMat::b3Inverse(A);
			/*XMVECTOR det;
			this->invMass = XMMatrixInverse(&det, A);
			XMFLOAT4 detF; XMStoreFloat4(&detF, det);
			if (fabs(detF.x) <= .000001f)
				this->invMass = XMMatrixIdentity();*/
				//this->invMass = MathUtil::xInverse(A);
			this->notInvMass = A;
			this->invMass = MathUtil::xInverse(A);
		}

		if (ResourceManager::constraintUseWarmStart)
		{
			this->object1->SetVelocity(XMVectorSubtract(this->object1->velocity, this->accumulatedImpulse));
			this->object1->SetPitchYawRollVelocity(XMVectorSubtract(this->object1->pitchYawRollVelocity,
				XMVector3Cross(this->leverArm1, this->accumulatedImpulse)));
		}

		this->accumulatedImpulse = XMVectorZero();
	}
}
