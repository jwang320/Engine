#include "pch.h"
#include "PerlinNoiseNode.h"

static int grad3[12][3] = { { 1,1,0 },{ -1,1,0 },{ 1,-1,0 },{ -1,-1,0 },
{ 1,0,1 },{ -1,0,1 },{ 1,0,-1 },{ -1,0,-1 },
{ 0,1,1 },{ 0,-1,1 },{ 0,1,-1 },{ 0,-1,-1 } };

static int fastfloor(double x) {
	return x > 0 ? static_cast<int>(x) : static_cast<int>(x) - 1;
}

static const int p[] = { 151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };

static int perm[512];

static bool initPerm()
{
	for (int a = 0; a < 512; ++a)
	{
		perm[a] = p[a & 255];
	}

	return true;
}

static const bool initPermV = initPerm();

static double kernel[9] =
{ 1, 6, 1,
6, 12, 6,
1, 6, 1 };

static bool initKernel()
{
	double total = 0;
	for (int a = 0; a < 9; ++a)
	{
		total += kernel[a];
	}

	for (int a = 0; a < 9; ++a)
	{
		kernel[a] /= total;
	}

	return true;
}

static bool kernelInit = initKernel();
static double defaultNoise(int x, int z)
{
	int n = x + z * 57;
	n = (n << 13) ^ n;
	return static_cast<double>(1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

static double defaultSmooth(int x, int z, const double kernel[9])
{
	//return defaultNoise(x, z);

	const double top = defaultNoise(x - 1, z + 1) * kernel[0] + defaultNoise(x, z + 1) * kernel[1] + defaultNoise(x + 1, z + 1) * kernel[2];
	const double middle = defaultNoise(x - 1, z) * kernel[3] + defaultNoise(x, z) * kernel[4] + defaultNoise(x + 1, z) * kernel[5];
	const double bottom = defaultNoise(x - 1, z - 1) * kernel[6] + defaultNoise(x, z - 1) * kernel[7] + defaultNoise(x + 1, z - 1) * kernel[8];

	const double total = (top + middle + bottom);
	return total;
}

namespace ProceduralGeneration
{
	double PerlinNoiseNode::compute(double x, double z)
	{
		double result = 0;

		result = this->computePerlinNoise(x, z);
		return result;
	}

	double PerlinNoiseNode::computePerlinNoise(double x, double z) const
	{
		double result = 0;

		for (int octave = 0; octave < this->octaveCount; ++octave)
		{
			const double newX = x * this->frequency[octave];
			const double newZ = z * this->frequency[octave];
			const int intX = static_cast<int>(newX);
			const int intZ = static_cast<int>(newZ);


			const double fX = newX - intX;
			const double fZ = newZ - intZ;

			const double v1 = defaultSmooth(intX, intZ, kernel);
			const double v2 = defaultSmooth(intX + 1, intZ, kernel);
			const double v3 = defaultSmooth(intX, intZ + 1, kernel);
			const double v4 = defaultSmooth(intX + 1, intZ + 1, kernel);

			const double i1 = defaultInterp(v1, v2, fX);
			const double i2 = defaultInterp(v3, v4, fX);

			const double randVal = defaultInterp(i1, i2, fZ);

			const double finalVal = randVal * this->amplitude[octave];

			const double rato = this->amplitude[octave] != 0 ? abs((finalVal + this->amplitude[octave]) / (this->amplitude[octave] * 2)) : 0;
			const double finalRatioValue = this->falloffPower != 0 ? pow(rato, this->falloffPower) : 1;
			result += finalVal * finalRatioValue;
		}

		return result;
	}
}
