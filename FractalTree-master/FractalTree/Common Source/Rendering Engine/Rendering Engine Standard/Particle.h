#pragma once
#include <DirectXMath.h>

namespace RenderingEngine
{
	class Particle
	{
	public:
		int lifeTime;
		DirectX::XMVECTOR location;
		DirectX::XMVECTOR velocity;
		DirectX::XMVECTOR acceleration;
		DirectX::XMVECTOR tint;
		bool active;
		float radius;

	public:
		Particle(int lifeTime = 100,
			const DirectX::XMVECTOR& location = DirectX::XMVectorSet(0, 0, 0, 0), 
			const DirectX::XMVECTOR& velocity = DirectX::XMVectorSet(0, 0, 0, 0),
			const DirectX::XMVECTOR& acceleration = DirectX::XMVectorSet(0, 0, 0, 0),
			const DirectX::XMVECTOR& tint = DirectX::XMVectorSet(1, 1, 1, 1));
		void Update();
	};
}