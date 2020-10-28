#pragma once
#include "NoiseNode.h"

namespace ProceduralGeneration
{
	class PerlinNoiseNode : public NoiseNode
	{
	public:
		using NoiseNode::NoiseNode; //this inherits NoiseNode's constructors.

	protected:
		virtual double compute(double x, double z) override;

	private:
		double computePerlinNoise(double x, double z) const;
	};
}