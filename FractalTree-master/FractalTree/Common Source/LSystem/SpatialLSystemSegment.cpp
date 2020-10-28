#include "pch.h"
#include "SpatialLSystemSegment.h"

using namespace DirectX;

SpatialLSystemSegment::SpatialLSystemSegment(LSystem* parentSystem, 
					  LSystemSegment* parent,
					  unsigned short segmentType,
					  unsigned short maxDepth, 
					  unsigned short currentDepth,
					  const M3SpatialLSystemParameters& parameters,
					  const XMVECTOR& endPoint,
					  const XMMATRIX& endTransformation,
					  const TrunkParameters& trunkParameters)
					  :	LSystemSegment(parentSystem, parent, segmentType, maxDepth, currentDepth),
					  length(parameters.length),
					  startRadius(parameters.startRadius),
					  endRadius(parameters.endRadius),
					  endTransformation(endTransformation),
					  startPoint(XMVector3Transform(XMVectorSet(0,0,0,0), parameters.segmentStart)),
					  endPoint(endPoint),					  
					  yRotation(parameters.yRotation),
					  totalRotationY(parameters.yRotation + parameters.totalYRotation),
					  trunkParameters(trunkParameters)
{

}
