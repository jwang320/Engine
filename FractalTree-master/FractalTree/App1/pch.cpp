#include "pch.h"

#ifndef _PERFDEFINED
#define _PERFDEFINED
double performanceFrequency;
#endif
void FailExit()
{
#ifdef _WIN32_APPLICATION
	exit(5);
#endif
}