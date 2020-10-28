#include "pch.h"
#pragma once
#include "LSystem.h"
#include "..\Terrain Simulation\TerrainResources.h"
#include "..\Terrain Simulation\Voxelizable.h"

struct TreeBranchParameters
{
	const int minBranchCount;
	const int maxBranchCount;
	const float zRotationMin;
	const float zRotationMax;
	const float minimumYSeparation;
	const float maximumYSeparation;
	const float radiusFactor;
	const float radiusFactorRange;

	TreeBranchParameters(int minBranchCount = 1,
		int maxBranchCount = 5,
		float zRotationMin = 0,
		float zRotationMax = 45,
		float minimumYSeparation = 20,
		float maximumYSeparation = 80,
		float radiusFactor = 1,
		float radiusFactorRange = 0) :
	minBranchCount(minBranchCount),
		maxBranchCount(maxBranchCount),
		zRotationMin(zRotationMin),
		zRotationMax(zRotationMax),
		minimumYSeparation(minimumYSeparation),
		maximumYSeparation(maximumYSeparation),
		radiusFactor(radiusFactor),
		radiusFactorRange(radiusFactorRange)
	{

	}
};

struct TrunkParameters
{
	const TreeBranchParameters branchParameters;
	const float radiusDeltaFactorMin;
	const float radiusFactorRange;
	const float lengthFactorMin;
	const float lengthFactorRange;
	const int trunkSegmentCount;
	const float zRotationMin;
	const float zRotationMax;
	const float yRotationMin;
	const float yRotationMax;

	TrunkParameters(const TreeBranchParameters& branchParameters = TreeBranchParameters(),
		float radiusDeltaFactorMin = 1,
		float radiusFactorRange = 0,
		float lengthFactorMin = 1,
		float lengthFactorRange = 0,
		int trunkSegmentCount = 5,
		float zRotationMin = 0,
		float zRotationMax = 20,
		float yRotationMin = 0,
		float yRotationMax = 360) :
	branchParameters(branchParameters),
		radiusDeltaFactorMin(radiusDeltaFactorMin),
		radiusFactorRange(radiusFactorRange),
		lengthFactorMin(lengthFactorMin),
		lengthFactorRange(lengthFactorRange),
		trunkSegmentCount(trunkSegmentCount),
		zRotationMin(zRotationMin),
		zRotationMax(zRotationMax),
		yRotationMin(yRotationMin),
		yRotationMax(yRotationMax)
	{

	}
};

struct M3SpatialLSystemParameters
{
	const DirectX::XMMATRIX& segmentStart;
	const DirectX::XMMATRIX& segmentRelativeRotation;
	float length;
	float startRadius;
	float endRadius;
	float xRotation;
	float yRotation;
	float zRotation;
	float totalYRotation;
	M3SpatialLSystemParameters(const DirectX::XMMATRIX& segmentStart, const DirectX::XMMATRIX& segmentRelativeRotation, float length, float startRadius, float endRadius, float xRotation = 0, float yRotation = 0, float zRotation = 0, float totalYRotation = 0) :
		segmentStart(segmentStart), segmentRelativeRotation(segmentRelativeRotation), length(length), startRadius(startRadius), endRadius(endRadius), xRotation(xRotation), yRotation(yRotation), zRotation(zRotation), totalYRotation(totalYRotation)
	{

	}
};

#define TRIANGLE_VOXEL_COUNT 8
class SpatialLSystemSegment : public LSystemSegment, public WorldGrowthSimulation::Voxelizable
{
	friend class SpatialLSystem;
protected:
	const float length;
	const float yRotation;
	const float totalRotationY;
	const DirectX::XMMATRIX endTransformation;
	const DirectX::XMVECTOR startPoint;
	const DirectX::XMVECTOR endPoint;
	const TrunkParameters trunkParameters;
	const float endRadius;

	const float growCost = .25f;
	const float branchCost = 4.f;
	float myResources = 0.0f;
	//float myBranchResources = 0.0f;
	float testWidthOffset = 0.0f;
	bool doneGrowing = false;
	const WorldGrowthSimulation::TerrainResources::Voxel* lastVoxel[TRIANGLE_VOXEL_COUNT];
	unsigned long long lastKey[TRIANGLE_VOXEL_COUNT] = { 0 };
	float safeDistance[TRIANGLE_VOXEL_COUNT] = { 0 };
	float lightCoefficient = 1.f;

private:
	const float startRadius;

public:
	SpatialLSystemSegment(LSystem* parentSystem, 
		LSystemSegment* parent,
		unsigned short segmentType,
		unsigned short maxDepth, 
		unsigned short currentDepth,
		const M3SpatialLSystemParameters& parameters,
		const DirectX::XMVECTOR& endPoint,
		const DirectX::XMMATRIX& endTransformation,
		const TrunkParameters& trunkParameters = TrunkParameters());

#pragma region Getters
	DirectX::XMMATRIX GetEndTransformation() const {return this->endTransformation;}
	DirectX::XMVECTOR GetStartPoint() const {return this->startPoint;}
	DirectX::XMVECTOR GetEndPoint() const {return this->endPoint;}
	float GetStartRadius() const {return this->startRadius + this->testWidthOffset * growFactor;}
	float GetEndRadius() const {return this->endRadius + this->testWidthOffset * growFactor;}
	float GetYRotation() const {return this->yRotation;}
	float GetTotalYRotation() const {return this->totalRotationY;}
	float GetBuildEndRadius() const { return this->endRadius; }
	float GetTestWidthOffset() const { return this->testWidthOffset; }
	float GetLightCoefficient() const { return this->lightCoefficient; }
	SpatialLSystemSegment* parentSegment() { return static_cast<SpatialLSystemSegment*>(this->parent); }
#pragma endregion
	void SetLightCoefficient(const float lightCoefficient) { this->lightCoefficient = lightCoefficient; }
	float getGrowthCost(const float r)
	{
		return 2.f * 3.1415f * this->testWidthOffset * this->testWidthOffset * this->growCost;
	}
	float getBranchCost()
	{
		return this->branchCost;// +this->currentDepth * .1f;
	}

	bool CanAffordGrowth(const float r)
	{
		return this->myResources >= this->getGrowthCost(r);
	}

	bool SetTestWidthOffset(const float tw) {
		if (this->needsIteration)
		{
			return false;
		}

		if ((this->parent == nullptr || this->testWidthOffset + tw <= this->parentSegment()->GetTestWidthOffset()))
		{
			const float cost = this->getGrowthCost(tw);
			if (cost <= this->myResources)
			{
				this->testWidthOffset += tw;
				this->myResources -= cost;
				return true;
			}
		}

		return false;
	}

	void addResources(const float amount) { this->myResources += amount; }
};