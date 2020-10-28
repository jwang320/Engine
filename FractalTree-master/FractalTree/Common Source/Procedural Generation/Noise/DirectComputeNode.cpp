#include "pch.h"
#include "DirectComputeNode.h"

namespace ProceduralGeneration
{
	DirectComputeNode::DirectComputeNode(std::function<double(double, double)> computeFunc, GenerationNode* parent, bool contributeValue, NoiseFunctionDependency dependency) :
		GenerationNode(parent, contributeValue, dependency),
		computeFunc(computeFunc)
	{
	}

	double DirectComputeNode::compute(double x, double z)
	{
		return this->computeFunc(x, z);
	}
}