#pragma once
#ifndef _BBOX
#define _BBOX
#endif
#include <DirectXMath.h>

namespace Geometry
{
	struct BoundingBox
	{
		DirectX::XMVECTOR minBounds;
		DirectX::XMVECTOR maxBounds;
	};

	struct Frustum
	{
		DirectX::XMVECTOR center;
		DirectX::XMVECTOR planes[6]; // near, far, left, right, up, down
		DirectX::XMVECTOR vertices[6];
	};

	bool CheckFrustumCollisionBoundingBox(const DirectX::XMVECTOR& nearPlane,
		const DirectX::XMVECTOR& farPlane,
		const DirectX::XMVECTOR& left,
		const DirectX::XMVECTOR& right,
		const DirectX::XMVECTOR& up,
		const DirectX::XMVECTOR& down,
		const BoundingBox& boundingBox);
	bool CheckCameraCollisionBoundingBox(const DirectX::XMVECTOR& cameraPosition, const DirectX::XMVECTOR& cameraFacing, const BoundingBox& boundingBox);
	bool CheckProjectionCollisionBoundingBox(const DirectX::XMVECTOR& cameraPosition, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const BoundingBox& boundingBox);
}