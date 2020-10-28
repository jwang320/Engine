#pragma once

namespace SimulationEngine
{
	class TestMat
	{
		//https://github.com/irlanrobson/Bounce-Lite/blob/master/Src/Common/Math/b3Mat33.h
	public:
		DirectX::XMVECTOR x;
		DirectX::XMVECTOR y;
		DirectX::XMVECTOR z;

		TestMat(const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2, const DirectX::XMVECTOR& v3) :
			x(v1), y(v2), z(v3) {

		}

		TestMat(){}

		static inline TestMat identity() {
			using namespace DirectX;
			return TestMat(
				XMVectorSet(1, 0, 0, 0),
				XMVectorSet(0, 1, 0, 0),
				XMVectorSet(0, 0, 1, 0));
		}

		static inline TestMat diagonal(const float d) {
			using namespace DirectX;
			return TestMat(
				XMVectorSet(d, 0, 0, 0),
				XMVectorSet(0, d, 0, 0),
				XMVectorSet(0, 0, d, 0));
		}

		static inline TestMat allOnes() {
			using namespace DirectX;
			return TestMat(
				XMVectorSet(1, 1, 1, 0),
				XMVectorSet(1, 1, 1, 0),
				XMVectorSet(1, 1, 1, 0));
		}

		static inline TestMat allOnes(const float s) {
			using namespace DirectX;
			return TestMat(
				XMVectorSet(s, s, s, 0),
				XMVectorSet(s, s, s, 0),
				XMVectorSet(s, s, s, 0));
		}

		static inline TestMat someOnes() {
			using namespace DirectX;
			return TestMat(
				XMVectorSet(1, 0, 0, 0),
				XMVectorSet(0, 1, 1, 0),
				XMVectorSet(0, 1, 1, 0));
		}

		static inline TestMat fromXMat(const DirectX::XMMATRIX& m)
		{
			return TestMat(m.r[0], m.r[1], m.r[2]);
		}

		static inline TestMat fromEigen(const Eigen::Matrix3d& mat)
		{
			return TestMat(DirectX::XMVectorSet(mat(0, 0), mat(0, 1), mat(0, 2), 0), 
				DirectX::XMVectorSet(mat(1, 0), mat(1, 1), mat(1, 2), 0), 
				DirectX::XMVectorSet(mat(2, 0), mat(2, 1), mat(2, 2), 0));
		}

		// Compute the inverse of a 3x3 matrix (A^-1).
		static inline TestMat b3Inverse(const TestMat& A) {
			using namespace DirectX;
			// Return the zero matrix if isn't invertible.
			const XMVECTOR c1 = XMVector3Cross(A.x, A.y);
			const XMVECTOR c2 = XMVector3Cross(A.y, A.z);
			const XMVECTOR c3 = XMVector3Cross(A.z, A.x);

			XMFLOAT4 det2;
			XMStoreFloat4(&det2, XMVector3Dot(c1, A.z));
			float det = det2.x;
#ifdef _DEBUG
			if (fabs(det) > .00001f) {
				det = 1 / det;
			} else {
				int br = 5;
			}
#else
			if (det != 0) {
				det = 1 / det;
			} else {
				int br = 5;
			}
#endif

			XMFLOAT4 c1F, c2F, c3F;
			XMStoreFloat4(&c1F, c1);
			XMStoreFloat4(&c2F, c2);
			XMStoreFloat4(&c3F, c3);

			return TestMat(
				XMVectorSet(det * c2F.x, det * c3F.x, det * c1F.x, 0),
				XMVectorSet(det * c2F.y, det * c3F.y, det * c1F.y, 0),
				XMVectorSet(det * c2F.z, det * c3F.z, det * c1F.z, 0));
		}

		static inline TestMat b3Transpose(const TestMat& A) {
			using namespace DirectX;
			XMFLOAT4 c1F, c2F, c3F;
			XMStoreFloat4(&c1F, A.x);
			XMStoreFloat4(&c2F, A.y);
			XMStoreFloat4(&c3F, A.z);
			return TestMat(
				XMVectorSet(c1F.x, c2F.x, c3F.x, 0),
				XMVectorSet(c1F.y, c2F.y, c3F.y, 0),
				XMVectorSet(c1F.z, c2F.z, c3F.z, 0)
			);
		}

		static inline TestMat b3SkewSymmetric(const DirectX::XMVECTOR& v) {
			using namespace DirectX;
			XMFLOAT4 vF;
			XMStoreFloat4(&vF, v);
			return TestMat(
				XMVectorSet(0, -vF.z, vF.y, 0),
				XMVectorSet(vF.z, 0, -vF.x, 0),
				XMVectorSet(-vF.y, vF.x, 0, 0));
		}

		static inline TestMat zero() {
			using namespace DirectX;
			return TestMat(XMVectorReplicate(0),
				XMVectorReplicate(0),
				XMVectorReplicate(0));
		}

		static inline TestMat FromQuaternion(const DirectX::XMVECTOR& q)
		{
			using namespace DirectX;
			return fromXMat(XMMatrixRotationQuaternion(q));
		}
	};
	// Compute u = Av.
	inline DirectX::XMVECTOR operator*(const TestMat& A, const DirectX::XMVECTOR& v) {
		using namespace DirectX;
		//return v.x * A.x + v.y * A.y + v.z * A.z;
		XMFLOAT4 vF;
		XMStoreFloat4(&vF, v);
		const XMVECTOR r1 = XMVectorMultiply(A.x, XMVectorReplicate(vF.x));
		const XMVECTOR r2 = XMVectorMultiply(A.y, XMVectorReplicate(vF.y));
		const XMVECTOR r3 = XMVectorMultiply(A.z, XMVectorReplicate(vF.z));
		return XMVectorMultiply(XMVectorAdd(XMVectorAdd(r1, r2), r3), XMVectorSet(1, 1, 1, 0));
	}
	// Compute C = AB.
	inline TestMat operator*(const TestMat& A, const TestMat& B) {
		using namespace DirectX;
		return TestMat(A * B.x, A * B.y, A * B.z);
	}

	// Compute C = A + B.
	inline TestMat operator+(const TestMat& A, const TestMat& B) {
		using namespace DirectX;
		return TestMat(
			XMVectorAdd(A.x, B.x),
			XMVectorAdd(A.y, B.y),
			XMVectorAdd(A.z, B.z));
	}
}