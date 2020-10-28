#pragma once

#include "..\Common Source\Simulation Engine\Physical Simulation\Actor.h"

namespace InverseKinematics { class Skeleton; class SkeletonSegment; }

namespace SimulationEngine
{
	class TestChainLink;

	class CharacterWithSkeleton : public Actor
	{
	protected:
		InverseKinematics::Skeleton* skeleton = nullptr;
		TestChainLink* theFirstChainLink;
		std::unordered_map<std::string, InverseKinematics::SkeletonSegment*> skeletonSegmentMap;
		std::unordered_map<Actor*, InverseKinematics::SkeletonSegment*> actorSegmentMap;
		std::vector<Actor*> chainLinks;

	public:
		CharacterWithSkeleton(World* const parentWorld, const MoveableParameters& moveableParameters = MoveableParameters());
		virtual void Act() override;
		void ResetSegments();
		~CharacterWithSkeleton();

		InverseKinematics::Skeleton* GetSkeleton() { return this->skeleton; }
		const InverseKinematics::Skeleton* GetSkeleton() const { return this->skeleton; }

	protected:
		void addLink(Actor* const chainLink, InverseKinematics::SkeletonSegment* const ikSegment);
		void orientActorToSegment(InverseKinematics::SkeletonSegment* const ikSegment);
		void orientAllActorsToSegments();
		void resetSegmentPositions();
	};
}