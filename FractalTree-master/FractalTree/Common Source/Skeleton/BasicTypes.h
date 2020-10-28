#pragma once
#include <DirectXMath.h>

namespace InverseKinematics
{
	typedef Eigen::MatrixXf MatrixType;
	typedef float FloatType;

	enum class GoalType : char
	{
		NONE = -1,
		TARGET_DISTANCE = 0,
		TARGET_FACING = 1, //dot product to target
		SEGMENT_HOLD_POSITION = 2, //hold relative position of segment at create time
		SEGMENT_ABSOLUTE_FACING = 3,
		SEGMENT_END_Y = 4 //y-coordinate of segment end point must be equal
	};

	struct TwoVectors
	{
		const DirectX::XMVECTOR startPoint;
		const DirectX::XMVECTOR endpoint;

		inline DirectX::XMVECTOR sub() const
		{
			return DirectX::XMVectorSubtract(this->endpoint, this->startPoint);
		}

		inline DirectX::XMVECTOR midpoint() const
		{
			return DirectX::XMVectorScale(DirectX::XMVectorAdd(this->startPoint, this->endpoint), .5f);
		}

		/*inline DirectX::XMVECTOR qBetween() const
		{
			using namespace DirectX;
			//XMquaternion
		}*/
	};

	inline float clampAngle(float ang)
	{
		while (ang > DirectX::XM_2PI)
		{
			ang -= DirectX::XM_2PI;
		}

		while (ang < 0)
		{
			ang += DirectX::XM_2PI;
		}

		return ang;
	}

	inline DirectX::XMVECTOR clampAngle(const DirectX::XMVECTOR& ang)
	{
		DirectX::XMFLOAT4 a;
		XMStoreFloat4(&a, ang);
		a.x = clampAngle(a.x);
		a.y = clampAngle(a.y);
		a.z = clampAngle(a.z);

		return DirectX::XMLoadFloat4(&a);
	}

	static inline DirectX::XMVECTOR Integrate(const DirectX::XMVECTOR& q, const DirectX::XMVECTOR& w) {
		using namespace DirectX;
		const XMVECTOR halfW = XMVectorMultiply(w, XMVectorReplicate(.5f));
		const XMVECTOR spin = XMQuaternionMultiply(q, halfW);
		return XMVectorAdd(q, spin);
	}
}
