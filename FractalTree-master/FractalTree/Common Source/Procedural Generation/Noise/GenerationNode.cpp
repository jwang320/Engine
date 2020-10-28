#include "pch.h"
#include "GenerationNode.h"

namespace ProceduralGeneration
{
	GenerationNode::GenerationNode(GenerationNode* parent,
		bool contributeValue,
		NoiseFunctionDependency dependency) :
		parent(parent),
		contributeValue(contributeValue),
		dependencyType(dependency.dependencyType),
		dependencyFailAction(dependency.dependencyFailAction),
		dependentValue1(dependency.dependentValue1),
		dependentValue2(dependency.dependentValue2)
	{
	}

	double GenerationNode::GetValue(double x, double z)
	{
		double myResult = this->compute(x, z);
		if (this->alwaysPositive)
		{
			myResult = fabs(myResult);
		}

		double result = this->contributeValue ? myResult : 0;

		for (auto node : this->children)
		{
			bool failDepndencyCheck = false;
			switch (node->dependencyType)
			{
			case NoiseDependencyType::COMPARE_LESS_EQUAL:
				failDepndencyCheck = myResult > node->dependentValue1;
				break;
			case NoiseDependencyType::COMPARE_GREATER_EQUAL:
				failDepndencyCheck = myResult < node->dependentValue1;
				break;
			}

			if (failDepndencyCheck)
			{
				switch (node->dependencyFailAction)
				{
				case NoiseDependencyFailAction::SKIP:
					continue;
				case NoiseDependencyFailAction::INTERPOLATE:
				{
					const double dif = fabs(node->dependentValue1 - myResult);
					if (dif > node->dependentValue2)
					{
						continue;
					}
					else
					{
						const double ratio = dif / node->dependentValue2;
						result += defaultInterp(node->GetValue(x, z), 0, ratio);
					}
				}
					break;
				}
			}
			else
			{
				result += node->GetValue(x, z);
			}		
		}

		return result;
	}

	void GenerationNode::SetSmoothingKernel(const double* const kernel, unsigned int kernelSize, double smoothingWidth)
	{
		this->smoothingWidth = smoothingWidth;
		this->kernelSize = kernelSize;
		this->smoothingKernel = new double[kernelSize];
		double total = 0;
		for (int a = 0; a < this->kernelSize; ++a)
		{
			total += kernel[a];
			this->smoothingKernel[a] = kernel[a];
		}

		for (int a = 0; a < this->kernelSize; ++a)
		{
			this->smoothingKernel[a] /= total;
		}
	}

	GenerationNode::~GenerationNode()
	{
		if (this->smoothingKernel != nullptr)
		{
			delete[] this->smoothingKernel;
		}
	}
}