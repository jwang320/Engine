#pragma once
#include <vector>
#include "ProceduralGeneration.h"

namespace ProceduralGeneration
{
	class GenerationNode
	{
	protected:
		GenerationNode* parent;
		std::vector<GenerationNode*> children;
		bool contributeValue;
		NoiseDependencyType dependencyType;
		NoiseDependencyFailAction dependencyFailAction;
		double dependentValue1;
		float dependentValue2;
		bool alwaysPositive = false;
		double* smoothingKernel = nullptr;
		double kernelSize = 0;
		double kernelWidth = .5;
		double smoothingWidth = .5;

	public:
		GenerationNode(GenerationNode* parent = nullptr,
			bool contributeValue = true,
			NoiseFunctionDependency dependency = NoiseFunctionDependency{});

		double GetValue(double x, double z);
		void AddChild(GenerationNode* child) { this->children.push_back(child); }

		virtual ~GenerationNode();

		void SetAlwaysPositive(bool alwaysPositive) { this->alwaysPositive = alwaysPositive; }
		void SetSmoothingKernel(const double* const kernel, unsigned int kernelSize = 9, double smoothingWidth = .5);

	protected:
		virtual double compute(double x, double z) = 0;
	};
}