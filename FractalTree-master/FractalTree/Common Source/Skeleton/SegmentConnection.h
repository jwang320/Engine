#pragma once

namespace InverseKinematics { class SkeletonSegment; }

//connection of segment to parent.
struct Connection
{
	DirectX::XMVECTOR offset;
	int dofCount = 0;
	bool xAvailable;
	bool yAvailable;
	bool zAvailable;
	DirectX::XMVECTOR pitchYawRoll = DirectX::XMVectorZero();
};