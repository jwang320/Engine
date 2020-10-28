#include "pch.h"
#include "Projectile.h"
#include "..\Common Source\Simulation Engine\Physical Simulation\PhysicalWorld.h"

using namespace DirectX;
using Eigen::MatrixXd;
using Eigen::JacobiSVD;

namespace SimulationEngine
{
	Projectile::Projectile(const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
		Actor(objectParameters, moveableParameters),
		constraintPosition(XMVectorAdd(moveableParameters.location, XMVectorSet(50, 50, 0, 0))),
		originalConstraintDistance(XMVector3Length(XMVectorSubtract(moveableParameters.location, constraintPosition)))
	{
		//this->location = XMVectorAdd(this->location, XMVectorSet(1000, -600, 1000, 0));
		//ApplyForceAtPoint(this->location, XMVectorMultiply(this->pitchYawRoll, XMVectorReplicate(5)));

		const float scaleFactor = .6f;//(rand() % 20 + 1) *.1f;
#ifndef _WIN8_1
		const float newScale = FractalTreeResources::BallScaleFactor*scaleFactor;
#else
		const float newScale = 2;
#endif
		this->scale = XMVectorSet(newScale, newScale, newScale, 1);
		this->mass = newScale*.1f;//;//powf(2, scaleFactor*5);

//		this->parentWorld->RegisterObjectProperty(this, PhysicalWorld::Physics, 1);
		this->componentFlags |= PhysicalWorld::PHYSICS;
		//this->velocity = XMVectorSet(20, 0, 0, 0);
		this->componentFlags |= PhysicalWorld::ACT;
		//this->parentWorld->RegisterObjectProperty(this, PhysicalWorld::Pathable, 1);
		//this->ApplyForceAtPoint(this->location, XMVectorSet(0, 10, 0, 0));
		this->lifeTime = 1000;
		this->objectType = ObjectTypes::Projectile;
		this->mass = 1;
		this->rotationalInertia = 1;
	}

	void Projectile::Act()
	{
		/*XMFLOAT4 constraintDistance;
		XMStoreFloat4(&constraintDistance, XMVectorSubtract(this->constraintPosition, this->location));

		MatrixXd jacobian(1, 12);
		jacobian.setZero();
		jacobian(0, 1) = -constraintDistance.x;
		jacobian(0, 2) = -constraintDistance.y;
		jacobian(0, 3) = -constraintDistance.z;

		XMFLOAT4 accumulatedForce;
		XMStoreFloat4(&accumulatedForce, this->appliedForce);
		MatrixXd forceExternal(12, 1);
		forceExternal.setZero();
		forceExternal(0, 1) = accumulatedForce.x;
		forceExternal(0, 2) = accumulatedForce.y;
		forceExternal(0, 3) = accumulatedForce.z;*/

		/*const XMVECTOR constraintDistance = XMVectorSubtract(this->constraintPosition, this->location);
		//const XMVECTOR weirdN1 = -XMVector3Dot(-constraintDistance, XMVectorAdd(this->velocity, this->appliedForce));
		const XMVECTOR b = XMVectorSubtract(XMVector3Dot(constraintDistance, constraintDistance),
			XMVector3Dot(this->originalConstraintDistance, this->originalConstraintDistance))*.5f;
		const XMVECTOR lambda = XMVectorDivide(-XMVectorAdd(XMVector3Dot(-constraintDistance, this->velocity), b), XMVector3Dot(constraintDistance, constraintDistance));
		//const XMVECTOR lambda = XMVectorDivide(-XMVector3Dot(-constraintDistance, this->velocity), XMVector3Dot(constraintDistance, XMVectorReplicate(1)));
		//const XMVECTOR deltaV = XMVectorMultiply(-constraintDistance, lambda*.005);
		//this->velocity = XMVectorAdd(this->velocity, deltaV);
		const XMVECTOR constraintForce = XMVectorMultiply(-constraintDistance, lambda);
		this->appliedForce = XMVectorAdd(this->appliedForce, constraintForce);*/
		//float lambda = 0;

		__super::Act();
		//--this->lifeTime;
		if(this->lifeTime < 0)
		{
			this->Remove();
		}
	}
}