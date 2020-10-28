#include "pch.h"

#ifndef _PERFDEFINED
#define _PERFDEFINED
double performanceFrequency;
void FailExit()
{
#ifdef _WIN32_APPLICATION
	exit(5);
#endif
}

std::string getVectorString(const DirectX::XMVECTOR& v)
{
	using namespace std;
	using namespace DirectX;
	XMFLOAT4 vF;
	XMStoreFloat4(&vF, v);

	const string s = "(" + to_string(vF.x) + ", " + to_string(vF.y) + ", " + to_string(vF.z) + ")";

	return s;
}

std::string getVectorStringDeg(const DirectX::XMVECTOR& v)
{
	using namespace std;
	using namespace DirectX;
	XMFLOAT4 vF;
	XMStoreFloat4(&vF, v);

	const string s = "(" + to_string(static_cast<int>(vF.x * 180 / 3.1415f)) + ", " + to_string(static_cast<int>(vF.y * 180 / 3.1415f)) + ", " + to_string(static_cast<int>(vF.z * 180 / 3.1415f)) + ")";

	return s;
}

float getVectorLen(const DirectX::XMVECTOR& v)
{
	using namespace DirectX;
	XMFLOAT4 vF;
	XMStoreFloat4(&vF, XMVector3Length(v));
	return vF.x;
}
#endif