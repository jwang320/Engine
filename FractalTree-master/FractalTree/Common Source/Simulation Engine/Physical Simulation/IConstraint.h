#pragma once
#include "TestMat.h"

namespace SimulationEngine
{
	class IConstraint
	{
	public:
		DirectX::XMMATRIX invMass = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX notInvMass = DirectX::XMMatrixIdentity();
		DirectX::XMVECTOR velocityBias = DirectX::XMVectorZero();
		DirectX::XMVECTOR accumulatedImpulse = DirectX::XMVectorZero();
	};

	inline DirectX::XMMATRIX skewSymmetric(const DirectX::XMVECTOR& v)
	{
		using namespace DirectX;
		XMFLOAT4 vF;
		XMStoreFloat4(&vF, v);
		return XMMatrixSet(
			0, -vF.z, vF.y, 0,
			vF.z, 0, -vF.x, 0,
			-vF.y, vF.x, 0, 0,
			0, 0, 0, 0);
	}

	/*static inline TestMat b3SkewSymmetric(const DirectX::XMVECTOR& v) {
			using namespace DirectX;
			XMFLOAT4 vF;
			XMStoreFloat4(&vF, v);
			return TestMat(
				XMVectorSet(0, -vF.z, vF.y, 0),
				XMVectorSet(vF.z, 0, -vF.x, 0),
				XMVectorSet(-vF.y, vF.x, 0, 0));
		}*/
}