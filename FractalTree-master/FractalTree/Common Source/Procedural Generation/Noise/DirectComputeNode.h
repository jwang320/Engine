#pragma once
#include "GenerationNode.h"
#include <functional>

namespace ProceduralGeneration
{
	//directly computes the result value from a given mathematical function.
	class DirectComputeNode : public GenerationNode
	{
	private:
		std::function<double(double, double)> computeFunc;

	public:
		DirectComputeNode(std::function<double(double, double)> computeFunc,
			GenerationNode* parent = nullptr,
			bool contributeValue = true,
			NoiseFunctionDependency dependency = NoiseFunctionDependency{});

		virtual double compute(double x, double z) override;
	};
}