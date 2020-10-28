#pragma once
#include <vector>
#include <DirectXMath.h>

namespace RenderingEngine
{
	class Particle;

	struct ParticleSystemDefinition
	{
		unsigned int maxConcurrentParticles;
		unsigned int emitCountMin;
		unsigned int emitCountRange;
		unsigned int emitCooldown;
		unsigned int particleDuration;
		unsigned int emitPerDirectionCount;
		unsigned int delay;
		float particleInitialSpeedMin;
		float particleInitialSpeedMax;
		float particleRadiusFactor;
		float particleRadius;
		float particleRadiusMin;
		DirectX::XMVECTOR particleAccelerationDirection;
		float particleInitialAccelerationMagnitude;

		DirectX::XMVECTOR particleCreationOffsetAngle;
		float particleCreationOffsetDistance;

		DirectX::XMVECTOR particleEmissionVelocityRotation;
		DirectX::XMFLOAT3 particleInitialVelocityRotationRange;
		std::vector<DirectX::XMVECTOR> particleEmissionDirection;
		bool rotateSystem;
		bool killSoon;
		int life;

		ParticleSystemDefinition(unsigned int maxConcurrentParticles = 100,
			unsigned int emitCooldown =20,
			unsigned int particleDuration = 3000,
			float particleInitialSpeedMin = .05f,
			float particleInitialSpeedMax = .05f,
			const DirectX::XMVECTOR& particleAccelerationDirection = DirectX::XMVectorSet(0, .01f, 0, 0),
			float particleInitialAcceleration = 0,//.1f,
			const DirectX::XMVECTOR& particleCreationOffsetAngle = DirectX::XMVectorSet(0, 0, 0, 0),
			float particleCreationOffsetDistance = 0,
			const DirectX::XMVECTOR& particleEmissionVelocityRotation = DirectX::XMVectorSet(0, 0, 0, 0),
			const DirectX::XMFLOAT3& particleInitialVelocityRotationRange = DirectX::XMFLOAT3(0, 0, 0),
			unsigned int emitPerDirectionCount = 1,
			float particleRadiusFactor = 1,
			float particleRadius = 1,
			unsigned int emitCountMin = 1,
			unsigned int emitCountRange = 0,
			unsigned delay = 100,
			float particleRadiusMin = 0,
			bool rotateSystem = false,
			bool killSoon = false,
			int life = 1) :
		
		maxConcurrentParticles(maxConcurrentParticles),
			emitCountMin(emitCountMin),
			emitCountRange(emitCountRange),
			emitCooldown(emitCooldown),
			particleDuration(particleDuration),
			particleInitialSpeedMin(particleInitialSpeedMin),
			particleInitialSpeedMax(particleInitialSpeedMax),
			particleAccelerationDirection(particleAccelerationDirection),
			particleInitialAccelerationMagnitude(particleInitialAcceleration),
			particleCreationOffsetAngle(particleCreationOffsetAngle),
			particleCreationOffsetDistance(particleCreationOffsetDistance),
			particleEmissionVelocityRotation(particleEmissionVelocityRotation),
			particleInitialVelocityRotationRange(particleInitialVelocityRotationRange),
			emitPerDirectionCount(emitPerDirectionCount),
			particleRadiusFactor(particleRadiusFactor),
			particleRadius(particleRadius),
			delay(delay),
			particleRadiusMin(particleRadiusMin),
			rotateSystem(rotateSystem),
			killSoon(killSoon),
			life(life)
		{

		}
	};

	class ParticleSystem
	{
	public:
		Particle* particles;
		unsigned int currentParticleCount;
		unsigned int currentFrame;
		DirectX::XMVECTOR location;
		float quadRadius;
		ParticleSystemDefinition particleSystemDefinition;
		bool useQuads;
		DirectX::XMVECTOR offset;
		bool started;
		DirectX::XMVECTOR particleSystemRotationOffset;
		DirectX::XMMATRIX transformation;
		bool enabled;
		DirectX::XMVECTOR velocity;

	public:
		ParticleSystem(const DirectX::XMVECTOR& location = DirectX::XMVectorSet(0, 0, 0, 0),
			const ParticleSystemDefinition& particleSystemDefinition = ParticleSystemDefinition(),
			float quadRadius = 8,
			const DirectX::XMVECTOR& offset = DirectX::XMVectorSet(0, 0, 0, 1));

		void Update();
		void CreateParticles();
		~ParticleSystem();


#ifndef _AMD64_
		void* operator new(size_t size)
		{
			return _aligned_malloc(size, 16);
		}
		void operator delete(void* p)
		{
			_aligned_free(p);
		}
		void* operator new[]( size_t n )
		{
			return operator new( n ) ;
		}

		void operator delete[]( void* p )
		{
			operator delete( p ) ;
		}
#endif
	};
}