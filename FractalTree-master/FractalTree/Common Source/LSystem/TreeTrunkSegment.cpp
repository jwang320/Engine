#include "pch.h"
#include "TreeTrunkSegment.h"
#include "SystemParameters.h"
#include "..\Terrain Simulation\TerrainResources.h"
#include "LSystem.h"

using namespace DirectX;
using namespace WorldGrowthSimulation;

bool TreeTrunkSegment::AddChildIteration()
{
	if (!this->needsIteration || this->myResources < this->getBranchCost())
	{
		return false;
	}

	this->myResources -= this->branchCost;

	//Branch at given intervals

	if(this->currentDepth % this->trunkParameters.trunkSegmentCount == 0 && this->currentDepth > 0)
	{
		if (this->maxBranchCount == -1)
		{
			const int branchCountRange = this->trunkParameters.branchParameters.maxBranchCount - this->trunkParameters.branchParameters.minBranchCount;
			const int branchCount = this->trunkParameters.branchParameters.minBranchCount + (branchCountRange != 0 ? (rand() % branchCountRange) : 0);
			this->maxBranchCount = branchCount;
		}

		float lastYAngle = 0;
		for(int a = 0; a < this->maxBranchCount && this->currentBranchCount < this->maxBranchCount; ++a)
		{
			const bool successful = this->createBranch();
			if (successful)
			{
				++this->currentBranchCount;
			}
		}
	}

	const int zRotRange = static_cast<int>((this->trunkParameters.zRotationMax - this->trunkParameters.zRotationMin) * 100) + failureCount * 25;
	float zRotation = zRotRange != 0 ? static_cast<float>((rand() % zRotRange)) : 0;
	zRotation = this->currentDepth > 0 ? (zRotation + this->trunkParameters.zRotationMin * 100) * .01f : 0.f;
	//zRotation = this->currentDepth > 0 && this->currentDepth %  3 == 0? 45 : 0;
	//const float yRotation = this->currentDepth > 0 && this->currentDepth %3 == 0? 36 : 0;//static_cast<float>(rand()%360);
	const int yRotRange = static_cast<int>((this->trunkParameters.yRotationMax - this->trunkParameters.yRotationMin));// +failureCount;
	const float yRotation = this->trunkParameters.yRotationMin + (yRotRange != 0 ? static_cast<float>(rand() % yRotRange) : 0);
	const int lenRangeT = static_cast<int>((this->trunkParameters.lengthFactorRange) * 100);
	const float newLength = this->length;//this->parentSystem->IsControlled() ? SystemParameters::LengthAmount :
		//this->length * ((lenRangeT > 0 ? ((rand() % lenRangeT) * .01f) : 0) + this->trunkParameters.lengthFactorMin);

	const int radRangeT = static_cast<int>((this->trunkParameters.radiusFactorRange) * 100);
	const float newRadius = this->GetBuildEndRadius();// *((radRangeT > 0 ? ((rand() % radRangeT) * .01f) : 0) + this->trunkParameters.radiusDeltaFactorMin);

	const BranchParameters newParams = BranchParameters(this->defaultParameters.newMaxDepth,
		zRotation,
		yRotation,
		newLength,
		newRadius);

	const bool success = this->continueTrunk(newParams, true);
	if (success)
	{
		return true;
	}

	++this->failureCount;
	return false;
}

bool TreeTrunkSegment::createBranch()
{
	const int zRotRange = static_cast<int>((this->trunkParameters.branchParameters.zRotationMax - this->trunkParameters.branchParameters.zRotationMin) * 100);
	float zRotation = zRotRange != 0 ? static_cast<float>((rand() % zRotRange)) : 0;
	zRotation = this->currentDepth > 0 ? (zRotation + this->trunkParameters.branchParameters.zRotationMin * 100) * .01f : 0.f;
	const int yRotRange = static_cast<const int>(this->trunkParameters.branchParameters.maximumYSeparation - this->trunkParameters.branchParameters.minimumYSeparation);
	const float yRotation = yRotRange != 0 ? this->trunkParameters.branchParameters.minimumYSeparation + static_cast<float>(rand() % yRotRange) + this->lastYAngle :
		this->lastYAngle + this->trunkParameters.branchParameters.minimumYSeparation;
	this->lastYAngle += yRotation;
	const int lenRangeT = static_cast<int>((this->trunkParameters.lengthFactorRange) * 100);
	const float newLength = this->length * ((lenRangeT > 0 ? ((rand() % lenRangeT) * .01f) : 0) + this->trunkParameters.lengthFactorMin);

	const int radRangeT = static_cast<int>((this->trunkParameters.radiusFactorRange) * 100);
	const float newRadius = (this->GetBuildEndRadius() * ((radRangeT > 0 ? ((rand() % radRangeT) * .01f) : 0) + this->trunkParameters.radiusDeltaFactorMin)) * this->trunkParameters.branchParameters.radiusFactor;

	const BranchParameters newParams = BranchParameters(this->defaultParameters.newMaxDepth,
		zRotation,
		yRotation,
		newLength,
		newRadius);

	return this->continueTrunk(newParams, false, 1.0f);
}

void TreeTrunkSegment::UpdateOther()
{
	if (this->parentSystem->currentBranches < this->parentSystem->maxBranches && !this->hasBranched && rand() % 1000 == 0)
	{
		this->createBranch();
		this->hasBranched = true;
		++this->parentSystem->currentBranches;
	}
}

bool TreeTrunkSegment::continueTrunk(const BranchParameters& params, bool connected, float branchSmallFactor)
{
	const float newSegmentLength = params.newLength;
	const float newStartRadius = this->GetBuildEndRadius() * branchSmallFactor;
	const float newEndRadius = params.newRadius;
	const unsigned int newMaxDepth = params.newMaxDepth;
	const unsigned int newCurrentDepth = this->currentDepth + 1;

	const float newZRotation = params.zRotation*3.14159265359f / 180;
	const float newYRotation = params.yRotation*3.14159265359f / 180;

	XMMATRIX newRelativeRotation = XMMatrixIdentity();
	const XMMATRIX newRootTransformation = this->endTransformation;

	newRelativeRotation = XMMatrixMultiply(newRelativeRotation, XMMatrixRotationZ(newZRotation));
	newRelativeRotation = XMMatrixMultiply(newRelativeRotation, XMMatrixRotationY(newYRotation));

	const XMVECTOR endPointV = XMVector3Transform(XMVectorSet(0, 0, 0, 0), XMMatrixMultiply(XMMatrixMultiply(XMMatrixTranslation(0, newSegmentLength, 0), newRelativeRotation), newRootTransformation));

	XMFLOAT4 endPoint;
	XMStoreFloat4(&endPoint, XMVectorAdd(endPointV, this->GetParentSystem()->getActor()->location));
	const XMMATRIX endTransformation = XMMatrixMultiply(XMMatrixMultiply(XMMatrixTranslation(0, newSegmentLength, 0), newRelativeRotation), newRootTransformation);
	const TerrainResources::Voxel* const voxel = FractalTreeResources::checkVoxels ?
		this->parentSystem->getTerrainResources()->GetVoxel(endPoint.x, endPoint.y, endPoint.z) :
		nullptr;

	if (voxel == nullptr || (voxel->objectType == 1 && static_cast<const SpatialLSystemSegment* const>(voxel->occupier)->GetParentSystem() == this->parentSystem))
	{
		TreeTrunkSegment* const newSegment = new TreeTrunkSegment(
			this->parentSystem,
			this,
			newMaxDepth,
			newCurrentDepth,
			M3SpatialLSystemParameters(
				newRootTransformation,
				newRelativeRotation,
				newSegmentLength,
				newStartRadius,
				newEndRadius,
				0,
				newYRotation,
				newZRotation,
				this->totalRotationY),
			endPointV,
			endTransformation,
			this->trunkParameters);
		if (voxel == nullptr && FractalTreeResources::checkVoxels)
		{
			this->parentSystem->getTerrainResources()->AddVoxel(endPoint.x, endPoint.y, endPoint.z, newSegment, 1);
		}

		this->neighbors.push_back(newSegment);

		if (!connected)
		{
			this->neighbors.back()->SetIsRoot(true);
			++this->parentSystem->rootNodeCount;
		}

		this->neighbors.back()->SetGrowFactor((connected ? 1.0f : .68f) * this->growFactor);
		return true;
	}
	else
	{
		return false;
	}	
}

TreeTrunkSegment::TreeTrunkSegment(LSystem* parentSystem, 
				 LSystemSegment* parent,
				 unsigned short maxDepth, 
				 unsigned short currentDepth, 
				 const M3SpatialLSystemParameters& parameters,
				 const XMVECTOR& endPoint,
				 const XMMATRIX& endTransformation,
				 const TrunkParameters& trunkParameters) :
	SpatialLSystemSegment(parentSystem, parent, 3, maxDepth, currentDepth, parameters, endPoint, endTransformation, trunkParameters)
{

}