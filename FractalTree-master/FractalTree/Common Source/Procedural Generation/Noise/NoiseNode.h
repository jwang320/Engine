#pragma once
#include "GenerationNode.h"

namespace ProceduralGeneration
{
	class NoiseNode : public GenerationNode
	{
	protected:
		std::vector<double> frequency;
		std::vector<double> amplitude;
		double falloffPower;
		const int octaveCount = 1;

	public:
		NoiseNode(GenerationNode* parent,
			bool contributeValue,
			NoiseFunctionDependency dependency,
			double frequency,
			double amplitude,
			int falloffPower = 0);
		NoiseNode(GenerationNode* parent,
			bool contributeValue,
			NoiseFunctionDependency dependency,
			double persistence,
			int octaveCount,
			int octaveStart = 0,
			double frequencyMultiplier = 1,
			double scale = 1, 
			int falloffPower = 0);
	};
}