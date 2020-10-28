#pragma once
#include <atomic>
#include "SegmentConnection.h"
#include "BasicTypes.h"

namespace InverseKinematics {
	class SkeletonSegment;

	class Skeleton
	{
	private:
		SkeletonSegment* rootSegment = nullptr;
		std::vector<SkeletonSegment*> segments;
		std::vector<SkeletonSegment*> segmentsWithGoals;
		std::vector<int> debugIndexes;
		unsigned int segmentCount = 0;
		unsigned int frameCount = 0;
		std::unordered_map<GoalType, std::vector<SkeletonSegment*>> goals;
		unsigned int totalGoalCount = 0;
		DirectX::XMVECTOR currentDestination = DirectX::XMVectorSet(0, 0, 0, 1);
		std::unordered_map<std::string, SkeletonSegment*> nameMap;
		unsigned int dofCount = 0; //number of total degrees of freedom for all segments combined
		unsigned int failFrames = 0;

	public:
		mutable std::atomic_bool hasChanged;
		static double desiredDist;

		Skeleton();
		void Update();
		unsigned int AddSegment(SkeletonSegment* const segment, const bool isRoot = true); //return segment id
		void deleteLastSegment();
		void SetDestination(const DirectX::XMVECTOR newDestination);
		void resetSegments();
		void ResetEndSegments();
		void resetSegmentsToBoundaries();
		void IncrementDofCount() { ++this->dofCount; }
		void DecrementDofCount() { --this->dofCount; }
		bool IsSolved() { return this->failFrames == 0; }
		~Skeleton();

	public:
		const std::vector<SkeletonSegment*>& GetSegments() const { return this->segments; }
		const std::vector<SkeletonSegment*>& GetEndSegments() const { return this->segmentsWithGoals; }
		unsigned int GetFailFrames() const { return this->failFrames; }

	private:
		//return a list of angle changes for this skeleton after running one iteration
		//of IK solver using the currently set destination from current skeleton configuration
		std::vector<DirectX::XMVECTOR> getAnglesForIKIteration() const;

		MatrixType getJacobian(const FloatType der) const;

		void Skeleton::computeJacobianRows(const DirectX::XMVECTOR& derDelta,
			const SkeletonSegment* const endSegment,
			const SkeletonSegment* const currentSegment,
			MatrixType& jacobian,
			const int rowIndex,
			const int columnIndex,
			const FloatType der) const;
	};
}