#include "pch.h"
#include "NoiseNode.h"

namespace ProceduralGeneration
{
	NoiseNode::NoiseNode(GenerationNode* parent,
		bool contributeValue,
		NoiseFunctionDependency dependency,
		double frequency,
		double amplitude,
		int falloffPower) :
		GenerationNode(parent, contributeValue, dependency),
		falloffPower(falloffPower)
	{
		this->frequency.push_back(frequency);
		this->amplitude.push_back(amplitude);
	}

	NoiseNode::NoiseNode(GenerationNode* parent,
		bool contributeValue,
		NoiseFunctionDependency dependency,
		double persistence,
		int octaveCount,
		int octaveStart,
		double frequencyMultiplier,
		double scale,
		int falloffPower) :
		GenerationNode(parent, contributeValue, dependency),
		falloffPower(falloffPower),
		octaveCount(octaveCount)
	{
		for (int i = 0; i < this->octaveCount; ++i)
		{
			const double frequency = powf(2.f, static_cast<double>(i + octaveStart + 1)) / scale * frequencyMultiplier;
			const double amplitude = powf(persistence, static_cast<double>(i + octaveStart + 1)) * scale;
			this->frequency.push_back(frequency);
			this->amplitude.push_back(amplitude);
		}
	}
}