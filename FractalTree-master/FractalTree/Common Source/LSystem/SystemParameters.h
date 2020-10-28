#pragma once
class SpatialLSystemSegment;

class SystemParameters
{
public:
	static float ConnectorThreshold;
	static float TrunkRadiusFactor;
	static float TrunkLengthFactor;
	static float TrunkStartRadius;
	static float TrunkStartLength;
	static float TrunkMinimumLength;
	static float TrunkMinimumRadius;
	static float TrunkBaseMinimumRadiusVariance;
	static float TrunkBaseMaximumRadiusVariance;
	static float CurveAmount;
	static float LengthAmount;
	static float ControlledLength;

	static int TrunkBaseRadiusVariance;
	static unsigned int MaxDepth;
	static bool ShowNormals;
	static bool IsTree;

	//Branching:
	static int MinimumBranchCount;
	static int MaximumBranchCount;
	static float BranchZMin;
	static float BranchZMax;
	static int ContinueTrunkChance;
	static float MinimumYSeparation;
	static float MaximumYSeparation;
	static float BranchRadiusFactor;
	static float BranchRadiusRange;

	//Trunk:
	static float radiusDeltaFactorMin;
	static float radiusFactorRange;
	static float lengthFactorMin;
	static float lengthFactorRange;
	static int trunkSegmentCount;
	static float zRotationMin;
	static float zRotationMax;
	static float yRotationMin;
	static float yRotationMax;

	static bool UseTrunkBaseVariance;
	static bool UseTrunkBodyVariance;
	static bool UseConnectorVariance;
	static bool AllowBranching;
	static bool Activated;
	static bool UseFaceNormals;
	static bool Paused;

	static bool UseControlledInput;
	static bool turnNow;
	static float (*TrunkRadiusFactorFunction)(const SpatialLSystemSegment*);
	static float (*TrunkLengthFactorFunction)(const SpatialLSystemSegment*);
	static float (*TrunkBaseVarianceFactorFunction)(const SpatialLSystemSegment*, const unsigned int index);
};