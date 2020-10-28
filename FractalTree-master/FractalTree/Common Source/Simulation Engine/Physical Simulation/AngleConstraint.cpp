#include "pch.h"
#include "AngleConstraint.h"
#include "UIRenderer.h"
#include "ResourceManager.h"

using namespace DirectX;

namespace SimulationEngine
{
	AngleConstraint::AngleConstraint(Actor* const object1, Actor* const object2) :
		object1(object1),
		object2(object2),
		originalConstraintDistance(XMVectorSubtract(object2->location, this->GetConstraintPoint1()))
	{
#ifndef _NO_GRAPHICS
		ind2 = RenderingEngine::UIRenderer::AddDebugString("Angle between objects: " + std::to_string(0));
#endif
	}
	AngleConstraint::AngleConstraint(Actor* const object1, const XMVECTOR& constraintPoint) :
		object1(object1),
		object2(nullptr),
		constraintPoint(constraintPoint),
		originalConstraintDistance(XMVectorSubtract(constraintPoint, this->GetConstraintPoint1()))
	{
#ifndef _NO_GRAPHICS
		ind2 = RenderingEngine::UIRenderer::AddDebugString("Angle between objects: " + std::to_string(0));
#endif
	}
	AngleConstraint::AngleConstraint(Actor* const object1, const XMVECTOR& constraintPoint, const XMVECTOR& offset1) :
		object1(object1),
		object2(nullptr),
		constraintPoint(constraintPoint),
		offset1(offset1),
		originalConstraintDistance(XMVectorSubtract(constraintPoint, this->GetConstraintPoint1()))
	{
#ifndef _NO_GRAPHICS
		ind2 = RenderingEngine::UIRenderer::AddDebugString("Angle between objects: " + std::to_string(0));
#endif
	}
	AngleConstraint::AngleConstraint(Actor* const object1,
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
#ifndef _NO_GRAPHICS
		ind2 = RenderingEngine::UIRenderer::AddDebugString("Angle between objects: " + std::to_string(0));
#endif
	}

	inline static DirectX::XMVECTOR toEulerianAngle(const DirectX::XMVECTOR& qq)
	{
		DirectX::XMFLOAT4 q;
		DirectX::XMStoreFloat4(&q, qq);
		double ysqr = q.y * q.y;
		double t0 = -2.0f * (ysqr + q.z * q.z) + 1.0f;
		double t1 = +2.0f * (q.x * q.y - q.w * q.z);
		double t2 = -2.0f * (q.x * q.z + q.w * q.y);
		double t3 = +2.0f * (q.y * q.z - q.w * q.x);
		double t4 = -2.0f * (q.x * q.x + ysqr) + 1.0f;

		t2 = t2 > 1.0f ? 1.0f : t2;
		t2 = t2 < -1.0f ? -1.0f : t2;

		const double pitch = std::asin(t2);
		const double roll = std::atan2(t3, t4);
		const double yaw = std::atan2(t1, t0);

		return DirectX::XMVectorSet(roll, pitch, yaw, 0);
	}

	inline static DirectX::XMVECTOR toEulerianAngle2(const DirectX::XMVECTOR& qq)
	{
		DirectX::XMFLOAT4 q1;
		DirectX::XMStoreFloat4(&q1, qq);
		const float test = q1.x*q1.y + q1.z*q1.w;

		float heading, attitude, bank;
		if (test > 0.499f) { // singularity at north pole
			heading = 2 * atan2(q1.x, q1.w);
			attitude = 3.1415f / 2;
			bank = 0;
		}
		else if (test < -0.499f) { // singularity at south pole
			heading = -2 * atan2(q1.x, q1.w);
			attitude = -3.1415f / 2;
			bank = 0;
		}
		else
		{
			float sqx = q1.x*q1.x;
			float sqy = q1.y*q1.y;
			float sqz = q1.z*q1.z;
			heading = atan2(2 * q1.y*q1.w - 2 * q1.x*q1.z, 1 - 2 * sqy - 2 * sqz);
			attitude = asin(2 * test);
			bank = atan2(2 * q1.x*q1.w - 2 * q1.y*q1.z, 1 - 2 * sqx - 2 * sqz);
		}

		//return DirectX::XMVectorSet(heading, attitude, bank, 0);
		return DirectX::XMVectorSet(heading, attitude, bank, 0);
	}
	inline static DirectX::XMVECTOR toEuler(float y, float z, float x, float angle) {
		float s = sinf(angle);
		float c = cosf(angle);
		float t = 1 - c;
		float heading, attitude, bank;
		//  if axis is not already normalised then uncomment this
		// double magnitude = Math.sqrt(x*x + y*y + z*z);
		// if (magnitude==0) throw error;
		// x /= magnitude;
		// y /= magnitude;
		// z /= magnitude;
		if ((x*y*t + z*s) > 0.998) { // north pole singularity detected
			heading = 2 * atan2(x*sinf(angle / 2), cosf(angle / 2));
			attitude = 3.1415f / 2;
			bank = 0;
			return DirectX::XMVectorSet(heading, attitude, bank, 0);
			//return XMVectorSet(attitude, heading, bank, 0);
		}
		if ((x*y*t + z*s) < -0.998) { // south pole singularity detected
			heading = -2 * atan2(x*sinf(angle / 2), cosf(angle / 2));
			attitude = -3.1415f / 2;
			bank = 0;
			return DirectX::XMVectorSet(heading, attitude, bank, 0);
			//return XMVectorSet(attitude, heading, bank, 0);
		}
		heading = atan2(y * s - x * z * t, 1 - (y*y + z*z) * t);
		attitude = asin(x * y * t + z * s);
		bank = atan2(x * s - y * z * t, 1 - (x*x + z*z) * t);

		return DirectX::XMVectorSet(heading, attitude, bank, 0);
		//return XMVectorSet(attitude, heading, bank, 0);
	}

	static const TestMat inertiaMatrix = 
		TestMat(
			DirectX::XMVectorSet(1, 0, 0, 0),
			DirectX::XMVectorSet(0, 1, 0, 0),
			DirectX::XMVectorSet(0, 0, 1, 0));

	void AngleConstraint::UpdateFrame()
	{
		using namespace DirectX;
		this->leverArm1 = this->GetLeverArm1();
		this->normalizedLeverArm1 = XMVector3Normalize(this->leverArm1);
		this->transformedPoint1 = XMVectorAdd(this->object1->location, leverArm1);

		this->leverArm2 = this->GetLeverArm2();
		this->normalizedLeverArm2 = XMVector3Normalize(this->leverArm2);
		this->transformedPoint2 = XMVectorAdd(this->object2->location, leverArm2);

		//const TestMat A = massMatrix + massMatrix +oneMatrix + oneMatrix;
		/*const TestMat inertiaMat1 = TestMat::diagonal(this->object1->GetRotationalInertia());
		const TestMat r1 = TestMat::FromQuaternion(this->object1->qOrientation);
		//const TestMat iA = TestMat::b3Inverse(r1 * inertiaMat1 * TestMat::b3Transpose(r1));
		const TestMat iA = TestMat::diagonal(1.f/this->object1->GetRotationalInertia());
		const TestMat massA = TestMat::diagonal(this->object1->GetInvMass());
		const TestMat skew1 = TestMat::b3SkewSymmetric(this->leverArm1);
		const TestMat skew1T = TestMat::b3Transpose(skew1);*/

		const XMMATRIX skew1 = skewSymmetric(this->leverArm1);
		const XMMATRIX skew1T = XMMatrixTranspose(skew1);
		const XMMATRIX massA = this->object1->inverseMassMatrix;
		const XMMATRIX iA = MathUtil::diagonal(this->object1->rotationalInertia);

		if (this->object2 != nullptr && !immovableObject2)
		{
			/*const TestMat inertiaMat2 = TestMat::diagonal(this->object2->GetRotationalInertia());
			const TestMat r2 = TestMat::FromQuaternion(this->object2->qOrientation);
			//const TestMat iB = TestMat::b3Inverse(r2 * inertiaMat2 * TestMat::b3Transpose(r2));
			const TestMat iB = TestMat::diagonal(1.f/this->object2->GetRotationalInertia());
			const TestMat massB = TestMat::diagonal(this->object2->GetInvMass());
			const TestMat skew2 = TestMat::b3SkewSymmetric(this->leverArm2);
			const TestMat skew2T = TestMat::b3Transpose(skew2);
			const TestMat A =  massA + massB + (skew1 * iA * skew1T) + (skew2 * iB * skew2T);
			this->invMass = TestMat::b3Inverse(A);*/
			const XMMATRIX skew2 = skewSymmetric(this->leverArm2);
			const XMMATRIX skew2T = XMMatrixTranspose(skew2);
			const XMMATRIX massB = this->object2->inverseMassMatrix;
			const XMMATRIX iB =  MathUtil::diagonal(this->object2->rotationalInertia);
			//const XMMATRIX A = MathUtil::add3(MathUtil::add3(massA, massB), MathUtil::add3(XMMatrixMultiply(skew1, XMMatrixMultiply(iA, skew1T)), XMMatrixMultiply(skew2, XMMatrixMultiply(iB, skew2T))));
			const XMMATRIX A = MathUtil::add3(MathUtil::add3(massA, massB), MathUtil::add3(XMMatrixMultiply(XMMatrixMultiply(skew1, iA), skew1T), XMMatrixMultiply(XMMatrixMultiply(skew2, iB), skew2T)));
			//const TestMat A = massA + massB + (skew1 * iA * skew1T) + (skew2 * iB * skew2T);
			//this->invMass = MathUtil::xInverse(A);
			this->notInvMass = A;
			this->invMass = MathUtil::xInverse(A);
			/*XMVECTOR det;
			this->invMass = XMMatrixInverse(&det, A);
			XMFLOAT4 detF; XMStoreFloat4(&detF, det);
			if (fabs(detF.x) <= .000001f || isnan(detF.x) || XMMatrixIsInfinite(this->invMass))
				this->invMass = XMMatrixIdentity();*/
		}
		else
		{
			/*const TestMat A = massA + (skew1 * iA * skew1T);
			this->invMass = TestMat::b3Inverse(A);*/
			const XMMATRIX A = MathUtil::add3(massA, XMMatrixMultiply(XMMatrixMultiply(skew1, iA), skew1T));
			//this->invMass = TestMat::b3Inverse(A);
			/*XMVECTOR det;
			this->invMass = XMMatrixInverse(&det, A);
			XMFLOAT4 detF; XMStoreFloat4(&detF, det);
			if (fabs(detF.x) <= .000001f || isnan(detF.x) || XMMatrixIsInfinite(this->invMass))
				this->invMass = XMMatrixIdentity();*/
				//this->invMass = MathUtil::xInverse(A);
			this->notInvMass = A;
			this->invMass = MathUtil::xInverse(A);
		}

		XMVECTOR targetQuaternion = this->object1->constraintTargetTest;

		const XMVECTOR quaternionBetweenObjects = XMQuaternionMultiply(this->object1->qOrientation, XMQuaternionInverse(this->object2->qOrientation));
		const XMVECTOR quaternionError = XMQuaternionMultiply(targetQuaternion, XMQuaternionInverse(quaternionBetweenObjects));

		XMVECTOR errorAxis;
		float scalarErrorAngle;
		XMQuaternionToAxisAngle(&errorAxis, &scalarErrorAngle, quaternionError);
		
		const XMVECTOR targetAngularVelocity = XMVectorMultiply(XMVector3Normalize(errorAxis), XMVectorReplicate(scalarErrorAngle));
		const XMVECTOR finalTargetBias = XMVectorMultiply(targetAngularVelocity, XMVectorReplicate(-ResourceManager::angleBB / (1.f / ResourceManager::timeStepCount)));

		this->velocityBias = XMVector3Rotate(finalTargetBias, this->object1->qOrientation);

		if (ResourceManager::constraintUseWarmStart)
		{
			this->object1->SetVelocity(XMVectorSubtract(this->object1->velocity, this->accumulatedImpulse));
			this->object1->SetPitchYawRollVelocity(XMVectorSubtract(this->object1->pitchYawRollVelocity,
				XMVector3Cross(this->leverArm1, this->accumulatedImpulse)));
		}

		this->accumulatedImpulse = XMVectorZero();
	}
}
