#include "pch.h"
#include "TestChainLink.h"
#include "..\Common Source\Skeleton\SkeletonSegment.h"

using namespace DirectX;
using Eigen::MatrixXd;

namespace SimulationEngine
{
	TestChainLink::TestChainLink(const ObjectParameters& objectParameters, const float length, const MoveableParameters& moveableParameters) :
		Actor(objectParameters, moveableParameters),
		length(length)
	{

		this->componentFlags |= PhysicalWorld::PHYSICS;
		this->componentFlags |= PhysicalWorld::ACT;

		//this->objectType = ObjectTypes::NOT_AN_OBJECT;
		this->objectType = ObjectTypes::TestChainLink;
		//this->scale = XMVectorSet(1, 20, 1, 0);
		this->ignoreForceEffects = FractalTreeResources::ignoreChainGravity;
		this->mass = 1;
		this->rotationalInertia = 1;

		CollisionHull* const newHull = new CollisionHull();
		newHull->minBounds = XMVectorSet(-2.5f, 0, -2.5f, 0);
		newHull->maxBounds = XMVectorSet(2.5f, 50, 2.5f, 0);
		this->collisionHulls.push_back(newHull);
		this->ignorePhysics = true;
	}

	void TestChainLink::Act()
	{
		if (this->pauseCount > 0)
		{
			//this->ApplyForceAtPoint(ePoint, XMVectorMultiply(eDir, XMVectorReplicate(.1f)), .05f);
			--this->pauseCount;
		}

		if (this->attachedSegment != nullptr) 
			this->constraintTargetTest = XMQuaternionRotationRollPitchYawFromVector(this->attachedSegment->GetPitchYawRoll());
		__super::Act();
	}
}
