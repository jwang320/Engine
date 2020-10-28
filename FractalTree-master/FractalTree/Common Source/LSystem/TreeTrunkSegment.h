#pragma once
#include "SpatialLSystemSegment.h"

class TreeTrunkSegment : public SpatialLSystemSegment
{
private:
	int failureCount = 0;
	int maxBranchCount = -1;
	int currentBranchCount = 0;
	int lastYAngle = 0;
	bool hasBranched = false;
	const BranchParameters defaultParameters = BranchParameters(this->maxDepth,
		0,
		0,
		this->length,
		this->GetBuildEndRadius());

public:
	TreeTrunkSegment(LSystem* parentSystem, 
		LSystemSegment* parent,
		unsigned short maxDepth, 
		unsigned short currentDepth, 
		const M3SpatialLSystemParameters& parameters,
		const DirectX::XMVECTOR& endPoint,
		const DirectX::XMMATRIX& endTransformation,
		const TrunkParameters& trunkParameters = TrunkParameters());

	virtual bool AddChildIteration() override;
	virtual void UpdateOther() override;

private:
	bool continueTrunk(const BranchParameters& parameters, bool connected = true, float branchSmallFactor = 1.0f);
	bool createBranch();
};