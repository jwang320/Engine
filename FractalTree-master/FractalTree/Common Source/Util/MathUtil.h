#pragma once

#include "../Common Source/Eigen/dense"

namespace MathUtil
{
	inline DirectX::XMMATRIX diagonal(const float v)
	{
		return DirectX::XMMatrixSet(v, 0, 0, 0,
			0, v, 0, 0,
			0, 0, v, 0,
			0, 0, 0, 1);
	}

	inline DirectX::XMMATRIX add3(const DirectX::XMMATRIX& m1, const DirectX::XMMATRIX& m2)
	{
		using namespace DirectX;
		return {XMVectorAdd(m1.r[0], m2.r[0]), XMVectorAdd(m1.r[1], m2.r[1]), XMVectorAdd(m1.r[2], m2.r[2]), m1.r[3]};
	}

	inline DirectX::XMMATRIX xInverse(const DirectX::XMMATRIX& A) {
		using namespace DirectX;
		// Return the zero matrix if isn't invertible.
		const XMVECTOR c1 = XMVector3Cross(A.r[0], A.r[1]);
		const XMVECTOR c2 = XMVector3Cross(A.r[1], A.r[2]);
		const XMVECTOR c3 = XMVector3Cross(A.r[2], A.r[0]);

		XMFLOAT4 det2;
		XMStoreFloat4(&det2, XMVector3Dot(c1, A.r[2]));
		float det = det2.x;
#ifdef _DEBUG
		if (fabs(det) > .00001f) {
			det = 1 / det;
		}
		else {
			int br = 5;
		}
#else
		if (det != 0) {
			det = 1 / det;
		}
		else {
			int br = 5;
		}
#endif

		XMFLOAT4 c1F, c2F, c3F;
		XMStoreFloat4(&c1F, c1);
		XMStoreFloat4(&c2F, c2);
		XMStoreFloat4(&c3F, c3);

		return XMMATRIX(
			XMVectorSet(det * c2F.x, det * c3F.x, det * c1F.x, 0),
			XMVectorSet(det * c2F.y, det * c3F.y, det * c1F.y, 0),
			XMVectorSet(det * c2F.z, det * c3F.z, det * c1F.z, 0), XMVectorZero());
	}

	inline Eigen::Matrix3f eFFromD(const DirectX::XMMATRIX& v)
	{
		using namespace DirectX;
		XMFLOAT4 r1, r2, r3;
		XMStoreFloat4(&r1, v.r[0]);
		XMStoreFloat4(&r2, v.r[1]);
		XMStoreFloat4(&r3, v.r[2]);
		Eigen::Matrix3f res;
		res(0, 0) = r1.x;
		res(0, 1) = r1.y;
		res(0, 2) = r1.z;
		res(1, 0) = r2.x;
		res(1, 1) = r2.y;
		res(1, 2) = r2.z;
		res(2, 0) = r3.x;
		res(2, 1) = r3.y;
		res(2, 2) = r3.z;
		return res;
	}

	inline DirectX::XMVECTOR dvFromE(const Eigen::Vector3f& v)
	{
		return DirectX::XMVectorSet(v.x(), v.y(), v.z(), 0);
	}

	inline Eigen::Vector3d fromDirectX(const DirectX::XMVECTOR& v)
	{
		using namespace DirectX;
		XMFLOAT4 vF; XMStoreFloat4(&vF, v);
		return Eigen::Vector3d(vF.x, vF.y, vF.z);
	}

	inline Eigen::Vector3f fromDirectXF(const DirectX::XMVECTOR& v)
	{
		using namespace DirectX;
		XMFLOAT4 vF; XMStoreFloat4(&vF, v);
		return Eigen::Vector3f(vF.x, vF.y, vF.z);
	}

	inline DirectX::XMVECTOR solveInv(const DirectX::XMMATRIX& A, const DirectX::XMVECTOR& b)
	{
		return dvFromE(eFFromD(A).colPivHouseholderQr().solve(fromDirectXF(b)));
	}
}