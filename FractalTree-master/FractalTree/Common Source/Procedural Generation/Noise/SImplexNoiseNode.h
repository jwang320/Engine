#pragma once
#include "NoiseNode.h"

namespace ProceduralGeneration
{
	class SimplexNoiseNode : public NoiseNode
	{
	public:
		using NoiseNode::NoiseNode; //this inherits NoiseNode's constructors.

	protected:
		virtual double compute(double x, double z) override;

	public:
		static double ComputeSimplexValue(double newX, double newZ, double ampl, double falloff);
	};
}