#pragma once
#include "..\Common Source\Simulation Engine\Physical Simulation\Actor.h"

namespace InverseKinematics { class Skeleton; class SkeletonSegment; }

namespace SimulationEngine
{
	class TestChainLink;

	class TestUserOfSkeleton : public Actor
	{
	private:
		int targetObjectID = -1;
		unsigned int frameCount = 2;
		InverseKinematics::Skeleton* skeleton = nullptr;

	public:
		TestUserOfSkeleton(const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters());
		virtual void Act() override;
		~TestUserOfSkeleton();

		virtual void TrackTarget(const int targetObjectId) override;
		DirectX::XMVECTOR destination;
		static float ikDistance;
		static bool fireNext;
		std::unordered_map<std::string, InverseKinematics::SkeletonSegment*> skeletonSegmentMap;
		const InverseKinematics::Skeleton* GetSkeleton() const { return this->skeleton; }
		void GenerateEverything2();
	private:
		void initializeCollisionHulls();
		void updateCollisionHulls();
	};
}