#include "pch.h"
#include "PerlinNoise.h"
#include "GenerationNode.h"
#include "SImplexNoiseNode.h"
#include "PerlinNoiseNode.h"
#include "DirectComputeNode.h"
#include <fstream>
using namespace DirectX;

namespace ProceduralGeneration
{
	PerlinHeightMapGenerator::PerlinHeightMapGenerator(bool doAnything)
	{
		if (!doAnything)
		{
			auto doNothing = [](double x, double z)
			{
				return 0;
			};
			DirectComputeNode* const nothingFunc = new DirectComputeNode(doNothing, nullptr, false, { NoiseDependencyType::NONE });
			this->generatorRootNodes.push_back(nothingFunc);
			return;
		}
		static double kernel2[9] =
		{ .25, 2, .25,
			2, 4, 2,
		.25, 2, .25 };
		
		SimplexNoiseNode* const testFunc = new SimplexNoiseNode(nullptr, false, { NoiseDependencyType::NONE }, .000005, 85500.0);
		SimplexNoiseNode* const testFunc2 = new SimplexNoiseNode(nullptr, false, {NoiseDependencyType::COMPARE_LESS_EQUAL, 0, 13000, NoiseDependencyFailAction::INTERPOLATE}, .000025, 8550.0);
		NoiseNode* const testFunc3 = new SimplexNoiseNode(nullptr, true, { NoiseDependencyType::COMPARE_GREATER_EQUAL, 4000, 8000, NoiseDependencyFailAction::INTERPOLATE }, .45, 8, 0, .25, 17500.0);
		testFunc3->SetSmoothingKernel(kernel2, 9, .5);
		
		auto cFunc = [](double x, double z)
		{
			const double fa = .000001;
			const double sc = 700;
			const double nX = x - 160000;
			const double nZ = z - 50000;
			if (abs(nX) > 5000 || abs(nZ) > 5000)
				return 0.0;
			const double sq = sqrtf(nX * nX + nZ * nZ);
			return sin(sq * .005) * sc;
			return sin((nX) * (nZ) * fa) * sc;//cosf(x*fa)*sinf(z*fa) * sc;//sinf((x + z)*.001f)*500;
		};

		auto canyonFunc = [](double x, double z)
		{
			x -= 70000;
			const double dist = abs(x - z) / sqrt(2.0);
			double closestX = -(-x - z) / 2.0;
			double closestZ = (x + z) / 2.0;
			double result = 0;
			if (dist > 30000)
				return dist;

			std::vector<double> frequencies(8);
			std::vector<double> amplitudes(8);
			const int octaveCount = 10;
			double persistence = 0.0;
			double sc = 5000.0;
			double freq = .15;
			if (x < z)
			{
				persistence = .25;
			}
			else
			{
				closestX += 100000;
				closestZ += 100000;
				persistence = .25;
			}
			for (int i = 0; i < octaveCount; ++i)
			{
				const double frequency = pow(2.0, static_cast<double>(i + 0)) / sc * freq;
				const double amplitude = pow(persistence, static_cast<double>(i + 0)) * sc;
				frequencies.push_back(frequency);
				amplitudes.push_back(amplitude);
			}

			for (int octave = 0; octave < octaveCount; ++octave)
			{
				const double newX = closestX * frequencies[octave];
				const double newZ = closestZ * frequencies[octave];
				result += SimplexNoiseNode::ComputeSimplexValue(newX, newZ, amplitudes[octave], 0);
			}

			return dist +result;
		};

		auto canyonFunc2 = [](double x, double z)
		{
			return -25000.0;
		};
		testFunc->AddChild(testFunc2);
		testFunc2->AddChild(testFunc3);
		DirectComputeNode* const directFunc = new DirectComputeNode(cFunc, nullptr, true, { NoiseDependencyType::COMPARE_GREATER_EQUAL, 250, 250, NoiseDependencyFailAction::INTERPOLATE });
		DirectComputeNode* const canyonFuncF = new DirectComputeNode(canyonFunc, nullptr, false, { NoiseDependencyType::NONE });
		DirectComputeNode* const canyonFuncVal = new DirectComputeNode(canyonFunc2, nullptr, true, { NoiseDependencyType::COMPARE_LESS_EQUAL, 11000, 18000, NoiseDependencyFailAction::INTERPOLATE });
		SimplexNoiseNode* const canyonBottom = new SimplexNoiseNode(nullptr, true, { NoiseDependencyType::COMPARE_LESS_EQUAL, 0, 1000, NoiseDependencyFailAction::INTERPOLATE }, .35, 8, 0, .25, 1750);
		canyonFuncF->AddChild(canyonFuncVal);
		canyonFuncVal->AddChild(canyonBottom);
		static double kernel4[9] =
		{ .25, 2, .25,
		2, 4, 2,
		.25, 2, .25 };
		canyonBottom->SetSmoothingKernel(kernel4);

		SimplexNoiseNode* const bigFunc = new SimplexNoiseNode(nullptr, false, { NoiseDependencyType::NONE }, .0000045, 305500.0);
		SimplexNoiseNode* const bigFuncVal = new SimplexNoiseNode(nullptr, true, { NoiseDependencyType::COMPARE_GREATER_EQUAL, 0, 103000, NoiseDependencyFailAction::INTERPOLATE }, .45, 12, 0, .5, 100000);// 68550.f);
		const double kernel3[9] =
		{ 1.75, 2, 1.75,
			2, 3, 2,
		1.75, 2, 1.75 };
		bigFuncVal->SetSmoothingKernel(kernel3, 9, .5);
		bigFunc->AddChild(bigFuncVal);
		this->generatorRootNodes.push_back(bigFunc);


		this->generatorRootNodes.push_back(directFunc);
		this->generatorRootNodes.push_back(testFunc);
		this->generatorRootNodes.push_back(canyonFuncF);
	}

	void PerlinHeightMapGenerator::GenerateHeightMap(float** heightMap, int width, int length, float gridWidth, float centerX, float centerZ)
	{
		for(int x = 0; x < width; ++x)
		{
			for(int z = 0; z < length; ++z)
			{
				heightMap[x][z] = this->GenerateHeightValue(x * gridWidth + centerX, z * gridWidth + centerZ);
			}
		}
	}

	void PerlinHeightMapGenerator::GenerateHeightMap(float** heightMap, int width, int length, float gridWidth, float centerX, float centerZ, float& lowest, float& highest)
	{
		for(int x = 0; x < width; ++x)
		{
			for(int z = 0; z < length; ++z)
			{
				heightMap[x][z] = this->GenerateHeightValue(x * gridWidth + centerX, z * gridWidth + centerZ);
				if(heightMap[x][z] < lowest)
				{
					lowest = heightMap[x][z];
				}
				else if(heightMap[x][z] > highest)
				{
					highest = heightMap[x][z];
				}
			}
		}
	}

	float PerlinHeightMapGenerator::GenerateHeightValue(float x, float z) const
	{
		float height = 0;

		for (auto generatorNode : this->generatorRootNodes)
		{
			height += generatorNode->GetValue(x, z);
		}

		return height;
	}
}