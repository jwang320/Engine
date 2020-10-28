#pragma once
#include "CharacterWithSkeleton.h"

namespace InverseKinematics { class Skeleton; class SkeletonSegment; }

namespace SimulationEngine
{
	class TestChainLink;

	class DynamicUserOfSkeleton : public CharacterWithSkeleton
	{
	private:
		int targetObjectID = -1;
		unsigned int frameCount = 2;

	public:
		DynamicUserOfSkeleton(const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters());
		void GenerateEverything();
		InverseKinematics::SkeletonSegment* MakeSegment(const std::string & name, const std::string& parentName, const float length,
			const bool xAvailable, const bool yAvailable, const bool zAvailable,
			InverseKinematics::SkeletonSegment* const parent, const bool refresh, const DirectX::XMVECTOR& pitchYawRoll = DirectX::XMVectorZero(),
			const DirectX::XMFLOAT4& minValues = { -1000, -1000, -1000, -1000 }, const DirectX::XMFLOAT4& maxValues = { 1000, 1000, 1000, 1000 });
		virtual void Act() override;
		~DynamicUserOfSkeleton();

		void TrackTarget(const int targetObjectId);
		DirectX::XMVECTOR destination;
	};
}
