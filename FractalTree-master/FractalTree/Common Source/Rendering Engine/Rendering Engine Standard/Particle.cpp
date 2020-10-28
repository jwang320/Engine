#include "pch.h"
#include "Particle.h"

using namespace DirectX;

namespace RenderingEngine
{
	Particle::Particle(int lifeTime, const XMVECTOR& location, const XMVECTOR& velocity, const XMVECTOR& acceleration, const XMVECTOR& tint) :
		lifeTime(lifeTime),
		location(location),
		velocity(velocity),
		acceleration(acceleration),
		tint(tint),
		active(false),
		radius(1)
	{

	}

	void Particle::Update()
	{
		this->velocity = XMVectorAdd(this->velocity, this->acceleration);
		this->location = XMVectorAdd(this->location, this->velocity);
		XMFLOAT4 pos;
		XMFLOAT4 dott;
		XMStoreFloat4(&dott, XMVector3Dot(this->velocity, XMVectorSet(0, 1, 0, 0)));
		XMStoreFloat4(&pos, this->location);

		if(pos.y < 0 && dott.y < 0)
		{
			this->velocity = XMVectorMultiply(XMVectorSet(1, -1, 1, 0), this->velocity);
		}

		--this->lifeTime;
	}
}