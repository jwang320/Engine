#pragma once

namespace ProceduralGeneration
{
	//The dependency conditions must PASS
	//to allow the noise function to execute
	enum class NoiseDependencyType : char
	{
		NONE,
		COMPARE_LESS_EQUAL,
		COMPARE_GREATER_EQUAL //parent value greater than my value
	};

	enum class NoiseDependencyFailAction : char
	{
		SKIP,
		INTERPOLATE
	}; 
	
	struct NoiseFunctionDependency
	{
		NoiseDependencyType dependencyType;
		double dependentValue1;
		double dependentValue2;
		NoiseDependencyFailAction dependencyFailAction;
		char dependencyIndex;
	};

	double defaultInterp(double a, double b, double x);
}