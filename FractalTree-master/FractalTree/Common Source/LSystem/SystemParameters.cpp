#include "pch.h"
#include "SpatialLSystemSegment.h"
#include "SystemParameters.h"

//Trunk Parameters
float SystemParameters::TrunkStartLength = 15.f;
float SystemParameters::TrunkStartRadius = 5;
float SystemParameters::radiusDeltaFactorMin = .9f;
float SystemParameters::radiusFactorRange = .05f;
float SystemParameters::lengthFactorMin = .95f;
float SystemParameters::lengthFactorRange = .035f;
int SystemParameters::trunkSegmentCount = 10;//5;
float SystemParameters::zRotationMin = 0;
float SystemParameters::zRotationMax = 10;//30;
float SystemParameters::yRotationMin = 0;
float SystemParameters::yRotationMax = 360;

//Branching Parameters
int SystemParameters::MinimumBranchCount = 0;//2;
int SystemParameters::MaximumBranchCount = 2;//3;
float SystemParameters::BranchZMin = 0;
float SystemParameters::BranchZMax = 35;//75;
int SystemParameters::ContinueTrunkChance = 0;
float SystemParameters::MinimumYSeparation = 5;//20;
float SystemParameters::MaximumYSeparation = 45;//80;
float SystemParameters::BranchRadiusFactor = 1;
float SystemParameters::BranchRadiusRange = .05f;

float SystemParameters::ConnectorThreshold = 1.25f;
float SystemParameters::TrunkLengthFactor = 1.f;
float SystemParameters::TrunkRadiusFactor = 1.f;
float SystemParameters::TrunkMinimumLength = 5.f;
float SystemParameters::TrunkMinimumRadius = 1.f;
float SystemParameters::TrunkBaseMinimumRadiusVariance = .75f;
float SystemParameters::TrunkBaseMaximumRadiusVariance = 2.5f;
float SystemParameters::CurveAmount = 25;
float SystemParameters::LengthAmount = 35;
float SystemParameters::ControlledLength = 15;
unsigned int SystemParameters::MaxDepth = 50;
int SystemParameters::TrunkBaseRadiusVariance = static_cast<int>((SystemParameters::TrunkBaseMaximumRadiusVariance - SystemParameters::TrunkBaseMinimumRadiusVariance) * 100);

bool SystemParameters::IsTree = true;
bool SystemParameters::UseTrunkBaseVariance = true;//true;
bool SystemParameters::UseTrunkBodyVariance = false;
bool SystemParameters::UseConnectorVariance = false;
bool SystemParameters::AllowBranching = false;
bool SystemParameters::ShowNormals = false;
bool SystemParameters::Activated = false;
bool SystemParameters::UseFaceNormals = false;
bool SystemParameters::Paused = false;
bool SystemParameters::UseControlledInput = false;
bool SystemParameters::turnNow = false;

float (*SystemParameters::TrunkRadiusFactorFunction)(const SpatialLSystemSegment*) = 
	[](const SpatialLSystemSegment* currentSegment)
{
	float newRadius = currentSegment->GetEndRadius() * SystemParameters::TrunkRadiusFactor;// / (currentSegment->GetCurrentDepth()*.5f + 1.f); 
	return newRadius > SystemParameters::TrunkMinimumRadius ? newRadius : SystemParameters::TrunkMinimumRadius;
};

float (*SystemParameters::TrunkLengthFactorFunction)(const SpatialLSystemSegment*) = 
	[](const SpatialLSystemSegment* currentSegment)
{
	float newLength = currentSegment->GetEndRadius() * SystemParameters::TrunkLengthFactor; 
	return newLength > SystemParameters::TrunkMinimumLength ? newLength : SystemParameters::TrunkMinimumLength;
};


inline float clamp(float val, float minVal, float maxVal)
{
	return val < minVal ? minVal : (val > maxVal ? maxVal : val);
}

float (*SystemParameters::TrunkBaseVarianceFactorFunction)(const SpatialLSystemSegment*, const unsigned int index) =
	[](const SpatialLSystemSegment* currentSegment, const unsigned int index)
{
	static const float variance[] = { 1.3f, 1.1f, 2.f, 1.f, 1.8f, 1.6f, .96f, .99f };
	return variance[index] * 1.3f;
	//float varianceAmount = 1.5;
	//float varianceAmount = !SystemParameters::UseTrunkBaseVariance ? 1.f : clamp((rand()%SystemParameters::TrunkBaseRadiusVariance)*.01f + 1, SystemParameters::TrunkBaseMinimumRadiusVariance, SystemParameters::TrunkBaseMaximumRadiusVariance);
	//return varianceAmount;
};