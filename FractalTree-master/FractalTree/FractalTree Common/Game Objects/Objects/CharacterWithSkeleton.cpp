#include "pch.h"
#include "CharacterWithSkeleton.h"
#include "..\Common Source\Skeleton\Skeleton.h"
#include "..\Common Source\Skeleton\SkeletonSegment.h"

using namespace DirectX;
using namespace InverseKinematics;

namespace SimulationEngine
{
	CharacterWithSkeleton::CharacterWithSkeleton(World* const parentWorld, const MoveableParameters& moveableParameters) :
		Actor(parentWorld, moveableParameters)
	{
	}

	void CharacterWithSkeleton::Act()
	{
		if (TestUserOfSkeleton::fireNext)
		{
			this->skeleton->Update();
		}
		__super::Act();
	}

	void CharacterWithSkeleton::ResetSegments()
	{
//		this->resetSegmentPositions();
		this->skeleton->ResetEndSegments();
		this->orientAllActorsToSegments();
		this->skeleton->hasChanged = true;
	}

	CharacterWithSkeleton::~CharacterWithSkeleton()
	{
	}

	void CharacterWithSkeleton::addLink(Actor* const chainLink, SkeletonSegment* const ikSegment)
	{
		this->chainLinks.push_back(chainLink);
		ikSegment->owningActor = chainLink;
		this->orientActorToSegment(ikSegment);
	}

	void CharacterWithSkeleton::orientActorToSegment(SkeletonSegment* const ikSegment)
	{
		if (ikSegment->owningActor == nullptr)
			return;

		//const TwoVectors endPoints = ikSegment->GetSegmentStartAndEnd();
		//ikSegment->owningActor->SetLocation(XMVectorAdd(this->GetLocation(), endPoints.midpoint()));

		const XMVECTOR qOrient = XMQuaternionRotationRollPitchYawFromVector(ikSegment->GetPitchYawRoll());
		ikSegment->owningActor->constraintTargetTest = XMQuaternionRotationRollPitchYawFromVector(XMVectorSet(0, 0, .1, 0));// (qOrient);
		//ikSegment->owningActor->SetPitchYawRoll(qOrient);
	}
	void CharacterWithSkeleton::orientAllActorsToSegments()
	{
		//this->resetSegmentPositions();
		for (SkeletonSegment* const segment : this->skeleton->GetSegments())
		{
			this->orientActorToSegment(segment);
		}
	}

	void CharacterWithSkeleton::resetSegmentPositions()
	{
		for (Actor* const a : this->chainLinks)
		{
			a->ResetPosition();
		}
	}
}
