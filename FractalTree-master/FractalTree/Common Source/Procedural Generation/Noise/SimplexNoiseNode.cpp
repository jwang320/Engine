#include "pch.h"
#include "SimplexNoiseNode.h"

static const int grad3[12][3] = { { 1,1,0 },{ -1,1,0 },{ 1,-1,0 },{ -1,-1,0 },
{ 1,0,1 },{ -1,0,1 },{ 1,0,-1 },{ -1,0,-1 },
{ 0,1,1 },{ 0,-1,1 },{ 0,1,-1 },{ 0,-1,-1 } };

static inline int fastfloor(double x) {
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

namespace ProceduralGeneration 
{
	double SimplexNoiseNode::compute(double x, double z)
	{
		double result = 0;

		for (int octave = 0; octave < this->octaveCount; ++octave)
		{
			const double newX = x * this->frequency[octave];
			const double newZ = z * this->frequency[octave];

			if (this->smoothingKernel != nullptr)
			{
				const double d = this->smoothingWidth / this->frequency[octave];
				result += SimplexNoiseNode::ComputeSimplexValue(newX, newZ, this->amplitude[octave], this->falloffPower) * this->smoothingKernel[4];
				result += SimplexNoiseNode::ComputeSimplexValue(newX - d, newZ - d, this->amplitude[octave], this->falloffPower) * this->smoothingKernel[0];
				result += SimplexNoiseNode::ComputeSimplexValue(newX, newZ - d, this->amplitude[octave], this->falloffPower) * this->smoothingKernel[1];
				result += SimplexNoiseNode::ComputeSimplexValue(newX + d, newZ - d, this->amplitude[octave], this->falloffPower) * this->smoothingKernel[2];
				result += SimplexNoiseNode::ComputeSimplexValue(newX - d, newZ, this->amplitude[octave], this->falloffPower) * this->smoothingKernel[3];
				result += SimplexNoiseNode::ComputeSimplexValue(newX + d, newZ, this->amplitude[octave], this->falloffPower) * this->smoothingKernel[5];
				result += SimplexNoiseNode::ComputeSimplexValue(newX - d, newZ + d, this->amplitude[octave], this->falloffPower) * this->smoothingKernel[6];
				result += SimplexNoiseNode::ComputeSimplexValue(newX, newZ + d, this->amplitude[octave], this->falloffPower) * this->smoothingKernel[7];
				result += SimplexNoiseNode::ComputeSimplexValue(newX + d, newZ + d, this->amplitude[octave], this->falloffPower) * this->smoothingKernel[8];
			}
			else
			{
				result += SimplexNoiseNode::ComputeSimplexValue(newX, newZ, this->amplitude[octave], this->falloffPower);//finalVal * finalRatioValue;
			}
		}

		return result;
	}

	double SimplexNoiseNode::ComputeSimplexValue(double newX, double newZ, double ampl, double falloff)
	{
		//const XMVECTOR newP = XMVectorSet(newX, 0, newZ, 0);
		double n0, n1, n2;
		static const double F2 = 0.5f*(sqrtf(3.0f) - 1.0f);

		const double s = (newX + newZ)*F2;
		const int i = fastfloor(newX + s);
		const int j = fastfloor(newZ + s);

		static const double G2 = (3.f - sqrtf(3.f)) / 6.f;
		const double t = (i + j) * G2;
		const double X0 = i - t; // Unskew the cell origin back to (x,z) space
		const double Z0 = j - t;
		const double x0 = newX - X0; // The x,z distances from the cell origin
		const double z0 = newZ - Z0;

		//const XMVECTOR X0 = XMVectorSet(i - t, 0, j - t, 0);
		// For the 2D case, the simplex shape is an equilateral triangle.
		// Determine which simplex we are in.
		int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
		if (x0 > z0) { i1 = 1; j1 = 0; } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
		else { i1 = 0; j1 = 1; } // upper triangle, YX order: (0,0)->(0,1)->(1,1)

								 // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
								 // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
								 // c = (3-sqrt(3))/6
		const double x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
		const double z1 = z0 - j1 + G2;
		const double x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
		const double z2 = z0 - 1.0f + 2.0f * G2;

		// Work out the hashed gradient indices of the three simplex corners
		const int ii = i & 255;
		const int jj = j & 255;
		const int gi0 = perm[ii + perm[jj]] % 12;
		const int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
		const int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;

		// Calculate the contribution from the three corners
		double t0 = 0.5f - x0*x0 - z0*z0;
		if (t0 < 0) n0 = 0.0f;
		else {
			t0 *= t0;
			//n0 = t0 * t0 * dot(grad3[gi0], x0, y0); // (x,y) of grad3 used for 2D gradient
			n0 = t0 * t0 * (grad3[gi0][0] * x0 + grad3[gi0][1] * z0);
		}

		double t1 = 0.5f - x1*x1 - z1*z1;
		if (t1 < 0) n1 = 0.0f;
		else {
			t1 *= t1;
			n1 = t1 * t1 * (grad3[gi1][0] * x1 + grad3[gi1][1] * z1);//dot(grad3[gi1], x1, y1);
		}

		double t2 = 0.5f - x2*x2 - z2*z2;
		if (t2 < 0) n2 = 0.0f;
		else {
			t2 *= t2;
			n2 = t2 * t2 * (grad3[gi2][0] * x2 + grad3[gi2][1] * z2);//dot(grad3[gi2], x2, z2);
		}


		const double finalVal = 70.0f * (n0 + n1 + n2) * ampl;

		const double rato = ampl != 0 ? abs((finalVal + ampl) / (ampl * 2)) : 0;
		const double finalRatioValue = falloff != 0 ? pow(rato, falloff) : 1;

		return finalVal * finalRatioValue;
	}
}
