#include "pch.h"
#include "BoundingBox.h"

using namespace DirectX;

namespace Geometry
{

	bool CheckCameraCollisionBoundingBox(const XMVECTOR& cameraPosition, const XMVECTOR& cameraFacing, const BoundingBox& boundingBox)
	{
		XMFLOAT4 ans;
		XMStoreFloat4(&ans, XMVectorGreater(boundingBox.maxBounds, cameraPosition));
		
		if(ans.x != 0 && ans.z != 0)
		{
			XMStoreFloat4(&ans, XMVectorLess(boundingBox.minBounds, cameraPosition));
			if(ans.x != 0 && ans.z != 0)
			{
				return true;
			}
		}

		const XMVECTOR center = XMVectorMultiply(XMVectorAdd(boundingBox.minBounds, boundingBox.maxBounds), XMVectorReplicate(.5f));
		XMFLOAT4 dotP;
		XMStoreFloat4(&dotP, XMVector3Dot(XMVectorSubtract(cameraPosition, center), cameraFacing));

		if(dotP.x < 0)
		{
			return true;
		}

		return false;
	}

	bool CheckProjectionCollisionBoundingBox(const XMVECTOR& cameraPosition, const XMMATRIX& view, const XMMATRIX& projection, const BoundingBox& boundingBox)
	{
		//return true;
		const XMVECTOR dimens = XMVectorSubtract(boundingBox.maxBounds, boundingBox.minBounds);
		XMFLOAT4 ans;
		XMStoreFloat4(&ans, XMVectorGreater(XMVectorAdd(boundingBox.maxBounds, dimens), cameraPosition));

		if(ans.x == 0xFFFFFFFF && ans.z == 0xFFFFFFFF)
		{
			XMStoreFloat4(&ans, XMVectorLess(XMVectorSubtract(boundingBox.minBounds, dimens), cameraPosition));
			if(ans.x == 0xFFFFFFFF && ans.z == 0xFFFFFFFF)
			{
				return true;
			}
		}

		XMFLOAT4 wid;
		XMStoreFloat4(&wid, dimens);

		const XMVECTOR center = XMVectorMultiply(XMVectorAdd(boundingBox.minBounds, boundingBox.maxBounds), XMVectorReplicate(.5f));
		const XMVECTOR topLeft = XMVectorAdd(boundingBox.minBounds, XMVectorSet(wid.x, 0, 0, 0));
		const XMVECTOR bottomRight = XMVectorMultiply(boundingBox.minBounds,XMVectorSet(0, 0, wid.x, 0));

		const XMMATRIX transform = XMMatrixMultiply(XMMatrixTranspose(view), XMMatrixTranspose(projection));


		const XMVECTOR screenSpaceCenter = XMVector4Transform(center, transform);
		
		//XMVECTOR cen = XMVectorDivide(screenSpaceCenter, XMVectorSplatW(screenSpaceCenter));
	

		XMStoreFloat4(&ans, XMVectorGreater(XMVectorSplatW(screenSpaceCenter), screenSpaceCenter));
		if(ans.x == 0xFFFFFFFF && ans.y == 0xFFFFFFFF && ans.z == 0xFFFFFFFF)
		{
			const XMVECTOR newSplat = XMVectorShiftLeft(-XMVectorSplatW(screenSpaceCenter), XMVectorSet(0, 0, 0, 0), 2);
			XMStoreFloat4(&ans, XMVectorGreater(screenSpaceCenter, newSplat));

			if(ans.x == 0xFFFFFFFF && ans.y == 0xFFFFFFFF && ans.z == 0xFFFFFFFF)
			{
				return true;
			}
		}

		//return false;
		const XMVECTOR screenSpaceMin = XMVector4Transform(boundingBox.minBounds, transform);
		const XMVECTOR screenSpaceMax = XMVector4Transform(boundingBox.maxBounds, transform);
		const XMVECTOR screenSpaceLeft = XMVector4Transform(topLeft, transform);
		const XMVECTOR screenSpaceRight = XMVector4Transform(bottomRight, transform);
		
		//XMVECTOR screenMinP = XMVectorDivide(screenSpaceMin, XMVectorSplatW(screenSpaceMin));
		//XMVECTOR screenMaxP = XMVectorDivide(screenSpaceMax, XMVectorSplatW(screenSpaceMax));

		XMStoreFloat4(&ans, XMVectorGreater(XMVectorSplatW(screenSpaceMin), screenSpaceMin));
		if(ans.x == 0xFFFFFFFF && ans.y == 0xFFFFFFFF && ans.z == 0xFFFFFFFF)
		{
			const XMVECTOR newSplat = XMVectorShiftLeft(-XMVectorSplatW(screenSpaceMin), XMVectorSet(0, 0, 0, 0), 2);
			XMStoreFloat4(&ans, XMVectorGreater(screenSpaceMin, newSplat));

			if(ans.x == 0xFFFFFFFF && ans.y == 0xFFFFFFFF && ans.z == 0xFFFFFFFF)
			{
				return true;
			}
		}

		XMStoreFloat4(&ans, XMVectorGreater(XMVectorSplatW(screenSpaceMax), screenSpaceMax));
		if(ans.x == 0xFFFFFFFF && ans.y == 0xFFFFFFFF && ans.z == 0xFFFFFFFF)
		{
			const XMVECTOR newSplat = XMVectorShiftLeft(-XMVectorSplatW(screenSpaceMax), XMVectorSet(0, 0, 0, 0), 2);
			XMStoreFloat4(&ans, XMVectorGreater(screenSpaceMax, newSplat));

			if(ans.x == 0xFFFFFFFF && ans.y == 0xFFFFFFFF && ans.z == 0xFFFFFFFF)
			{
				return true;
			}
		}

		XMStoreFloat4(&ans, XMVectorGreater(XMVectorSplatW(topLeft), topLeft));
		if(ans.x == 0xFFFFFFFF && ans.y == 0xFFFFFFFF && ans.z == 0xFFFFFFFF)
		{
			const XMVECTOR newSplat = XMVectorShiftLeft(-XMVectorSplatW(topLeft), XMVectorSet(0, 0, 0, 0), 2);
			XMStoreFloat4(&ans, XMVectorGreater(topLeft, newSplat));

			if(ans.x == 0xFFFFFFFF && ans.y == 0xFFFFFFFF && ans.z == 0xFFFFFFFF)
			{
				return true;
			}
		}

		XMStoreFloat4(&ans, XMVectorGreater(XMVectorSplatW(bottomRight), bottomRight));
		if(ans.x == 0xFFFFFFFF && ans.y == 0xFFFFFFFF && ans.z == 0xFFFFFFFF)
		{
			const XMVECTOR newSplat = XMVectorShiftLeft(-XMVectorSplatW(bottomRight), XMVectorSet(0, 0, 0, 0), 2);
			XMStoreFloat4(&ans, XMVectorGreater(bottomRight, newSplat));

			if(ans.x == 0xFFFFFFFF && ans.y == 0xFFFFFFFF && ans.z == 0xFFFFFFFF)
			{
				return true;
			}
		}
		return false;
	}

	bool CheckFrustumCollisionBoundingBox(
		const XMVECTOR& nearPlane,
		const XMVECTOR& farPlane,
		const XMVECTOR& left,
		const XMVECTOR& right,
		const XMVECTOR& up,
		const XMVECTOR& down,
		const BoundingBox& boundingBox)
	{
		return true;
	}

}