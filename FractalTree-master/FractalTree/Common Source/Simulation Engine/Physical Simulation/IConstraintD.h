#pragma once
#include "TestMat.h"
#include <algorithm>

namespace SimulationEngine
{
	class IConstraintD
	{
	public:
		Eigen::Vector3d velocityBias = Eigen::Vector3d(0, 0, 0);
		Eigen::Matrix3d invMass;
		Eigen::Vector3d accumulatedImpulse = Eigen::Vector3d(0, 0, 0);
	};

	inline DirectX::XMVECTOR fromEigen(const Eigen::Vector3d& v)
	{
		return DirectX::XMVectorSet(v[0], v[1], v[2], 0);
	}

	inline Eigen::Vector3d clampV(const Eigen::Vector3d& v, const Eigen::Vector3d& minV, const Eigen::Vector3d& maxV)
	{
		return Eigen::Vector3d(std::clamp(v[0], minV[0], maxV[0]), std::clamp(v[1], minV[1], maxV[1]), std::clamp(v[2], minV[2], maxV[2]));
	}

	inline Eigen::Quaterniond qFromD(const DirectX::XMVECTOR& q)
	{
		using namespace DirectX;
		XMFLOAT4 qF; XMStoreFloat4(&qF, q);
		return Eigen::Quaterniond(qF.w, qF.x, qF.y, qF.z);
	}

	inline DirectX::XMVECTOR dFromQ(const Eigen::Quaterniond& q)
	{
		return DirectX::XMVectorSet(q.x(), q.y(), q.z(), q.w());
	}

	inline Eigen::Matrix3d skewSymmetric(const Eigen::Vector3d& v)
	{
		auto s = Eigen::Matrix3d();
		s(0, 0) = 0;
		s(0, 1) = -v[2];
		s(0, 2) = v[1];
		s(1, 0) = v[2];
		s(1, 1) = 0;
		s(1, 2) = -v[0];
		s(2, 0) = -v[1];
		s(2, 1) = v[0];
		s(2, 2) = 0;
		return s;
		/*return TestMat(
		XMVectorSet(0, -vF.z, vF.y, 0),
		XMVectorSet(vF.z, 0, -vF.x, 0),
		XMVectorSet(-vF.y, vF.x, 0, 0));*/
	}
}