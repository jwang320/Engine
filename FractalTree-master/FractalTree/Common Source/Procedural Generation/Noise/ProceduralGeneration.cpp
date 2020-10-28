#include "pch.h"
#include "ProceduralGeneration.h"
#include <cmath>

namespace ProceduralGeneration
{
	double defaultInterp(double a, double b, double x)
	{
		const double f = (1.0 - cos(x * 3.1415927)) * .5;
		return  a*(1.0 - f) + b*f;
	}
}