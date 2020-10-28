#include "pch.h"
#include "DynamicUserOfSkeleton.h"
#include "..\Common Source\Skeleton\SkeletonSegment.h"
#include "..\Common Source\Skeleton\Skeleton.h"

using namespace SimulationEngine;
using namespace InverseKinematics;
using namespace DirectX;
using namespace std;

DynamicUserOfSkeleton::DynamicUserOfSkeleton(const ObjectParameters & objectParameters, const MoveableParameters & moveableParameters) :
	CharacterWithSkeleton(objectParameters.parentWorld, moveableParameters),
	destination(XMVectorSet(5, 60, 10, 1))
{
	this->collisionType = NONE;

	this->componentFlags |= PhysicalWorld::PHYSICS;
	this->componentFlags |= PhysicalWorld::PATH;

	this->ignoreForceEffects = true;
	this->objectType = ObjectTypes::DynamicSkeleton;
	this->GenerateEverything();
}

void DynamicUserOfSkeleton::GenerateEverything()
{
	this->skeleton = new InverseKinematics::Skeleton();

	for (SkeletonSegment* const segment : this->skeleton->GetSegments())
	{
		if (!segment->parentName.empty())
		{
			SkeletonSegment* const parent = this->skeletonSegmentMap.at(segment->parentName);
			segment->parentSegment = parent;
			parent->childSegments.push_back(segment);
		}
	}
	//this->theFirstChainLink = static_cast<TestChainLink*>(this->skeleton->segmentOwners[0]);
}

int segCount = 0;

SkeletonSegment* DynamicUserOfSkeleton::MakeSegment(
	const string& name,
	const string& parentName,
	 float length,
	const bool xAvailable,
	const bool yAvailable,
	const bool zAvailable,
	SkeletonSegment* const parent,
	const bool refresh,
	const XMVECTOR& pitchYawRoll,
	const XMFLOAT4& minValues,
	const XMFLOAT4& maxValues)
{
	//length = 50;
	const XMVECTOR offset = XMVectorSet(0, 0, 0, 1);
	const int doFCount = (xAvailable ? 1 : 0) + (yAvailable ? 1 : 0) + (zAvailable ? 1 : 0);
	const Connection connection = { offset, doFCount, xAvailable, yAvailable, zAvailable, pitchYawRoll};
	SkeletonSegment* const newSegment = new SkeletonSegment(this->skeleton, parent, length, false,
		false, minValues, maxValues, name, "", connection, parentName);

	if (name != "") {
		this->skeletonSegmentMap[name] = newSegment;
	}

	this->ResetSegments();

	if (parent != nullptr)
	{
		const TwoVectors endPoints = newSegment->GetSegmentStartAndEnd();
		//ikSegment->owningActor->SetLocation(XMVectorAdd(this->GetLocation(), endPoints.midpoint()));
		TestChainLink* const link = new TestChainLink(ObjectParameters(this->GetParentWorld()), newSegment->GetLength(),
			MoveableParameters(XMVectorAdd(this->location, endPoints.midpoint()), newSegment->GetCompoundPitchYawRoll()));
		link->SetMass(1);
		//const XMVECTOR qOrient = XMQuaternionRotationRollPitchYawFromVector(newSegment->GetPitchYawRoll());
		this->addLink(link, newSegment);
		link->attachedSegment = newSegment;
		//link->constraintTargetTest = qOrient;
		if (parent->owningActor != nullptr)
		{
			this->world->AddConstraint(parent->owningActor, link, XMVectorSet(0, parent->GetLength()/2, 0, 0), 
				 XMVectorSet(0, -newSegment->GetLength()/2, 0, 0));
			//if (this->chainLinks.size() > 2)
			this->world->AddAngleConstraint(link, parent->owningActor, XMVectorSet(0, -newSegment->GetLength()/2, 0, 0),
				XMVectorSet(0, parent->GetLength()/2, 0, 0));
		}
		else
		{
			this->world->AddConstraint(link, this->location, XMVectorSet(0, -newSegment->GetLength()/2, 0, 0));
			//this->world->AddAngleConstraint(link, XMVectorAdd(this->GetLocation(), XMVectorSet(0, -newSegment->GetLength() / 2, 0, 0)));
			//this->world->AddAngleConstraint(link, XMVectorSet(0, -length / 2, 0, 1), XMVectorZero(), true);
			this->world->AddAngleConstraint(link, this, XMVectorSet(0, -length / 2, 0, 1), XMVectorZero(), true);
			//const XMVECTOR qOrient = XMQuaternionRotationRollPitchYawFromVector(XMVectorSet(0, 3.1415f,0, 0));
		}
	}

	return newSegment;
}

void DynamicUserOfSkeleton::Act()
{
	const Actor* const target = static_cast<Actor*>(static_cast<const PhysicalWorld* const>(this->parentWorld)->GetObjectByGlobalID(this->targetObjectID));
	if (target == nullptr)
	{
		return;
	}

	this->destination = XMVectorSet(0, -50, 100, 0);
	XMVECTOR offsetDest = XMVectorSubtract(target->location, this->location);
	offsetDest = XMVectorSetW(offsetDest, 1);
	offsetDest = XMVectorAdd(offsetDest, XMVectorSet(0, 20, 0, 0));
	this->destination = XMVectorAdd(XMVectorMultiply(XMVectorReplicate(TestUserOfSkeleton::ikDistance), target->GetFacing()), offsetDest);
	this->skeleton->SetDestination(this->destination);
	
	if (this->skeleton->IsSolved())
	{
		this->orientAllActorsToSegments();
	}
	//XMQuaternionSlerp()
	__super::Act();
}

void DynamicUserOfSkeleton::TrackTarget(const int targetObjectId)
{
	this->targetObjectID = targetObjectId;
}

DynamicUserOfSkeleton::~DynamicUserOfSkeleton()
{
	if (this->skeleton != nullptr)
	{
		delete this->skeleton;
	}
}
