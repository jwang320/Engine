#include "pch.h"
#include "ParticleSystem.h"
#include "Particle.h"

using namespace DirectX;
namespace RenderingEngine
{
	ParticleSystem::ParticleSystem(const XMVECTOR& location, const ParticleSystemDefinition& particleSystemDefinition, float quadRadius, const XMVECTOR& offset) :
		currentParticleCount(0),
		currentFrame(0),
		location(location),	
		particleSystemDefinition(particleSystemDefinition),
		quadRadius(quadRadius),
		useQuads(true),
		offset(offset),
		started(false),
		particleSystemRotationOffset(XMVectorSet(0, 0, 0, 0)),
		transformation(XMMatrixIdentity()),
		enabled(true),
		velocity(XMVectorSet(0, 0, 0, 0))
	{
		//Allocate all particles at start of particle system's lifetime.
		this->particles = new Particle[this->particleSystemDefinition.maxConcurrentParticles];
	}

	void ParticleSystem::Update()
	{
		if(this->currentFrame >= this->particleSystemDefinition.emitCooldown && (this->started || this->currentFrame > this->particleSystemDefinition.delay))
		{
			this->started = true;
			this->currentFrame = 0;
			this->CreateParticles();
			if(this->particleSystemDefinition.killSoon)
			{
				--this->particleSystemDefinition.life;
			}
		}

		for(unsigned int a = 0; a < this->currentParticleCount; ++a)
		{
			if(this->particles[a].active)
			{
				if(this->particles[a].lifeTime <= 0)
				{
					--this->currentParticleCount;

					if(a < this->currentParticleCount)
					{					
						this->particles[a] = this->particles[this->currentParticleCount];

						//We want to be sure to not skip this moved particle, so decrement loop counter.
						//The moved particle will be updated next iteration.
						--a;
					}

					this->particles[this->currentParticleCount].active = false;
				}
				else
				{
					this->particles[a].Update();
					if(this->particles[a].radius > this->particleSystemDefinition.particleRadiusMin)
						this->particles[a].radius *= this->particleSystemDefinition.particleRadiusFactor;
				}
			}
		}

		++this->currentFrame;
	}

	void ParticleSystem::CreateParticles()
	{
		if(this->particleSystemDefinition.life < 0 || !this->enabled)
		{
			return;
		}
		unsigned int currentEmitCount = this->particleSystemDefinition.emitCountMin + (this->particleSystemDefinition.emitCountRange > 0 ? (rand() % this->particleSystemDefinition.emitCountRange) : 0);
		for(unsigned int a = 0; a < currentEmitCount && this->currentParticleCount < this->particleSystemDefinition.maxConcurrentParticles; ++a)
		{
			for(unsigned int whichDirection = 0; whichDirection < this->particleSystemDefinition.particleEmissionDirection.size() && this->currentParticleCount < this->particleSystemDefinition.maxConcurrentParticles; ++whichDirection)
			{
				for(unsigned int whichParticle = 0; whichParticle < this->particleSystemDefinition.emitPerDirectionCount && this->currentParticleCount < this->particleSystemDefinition.maxConcurrentParticles; ++whichParticle)
				{
					//Particles are already allocated; use the particle at the first unused index;
					const XMVECTOR newVelocityRotation = XMVectorAdd(this->particleSystemDefinition.particleEmissionVelocityRotation,
						XMVectorMultiply(XMVectorSet(rand() % (static_cast<int>(this->particleSystemDefinition.particleInitialVelocityRotationRange.x)) - this->particleSystemDefinition.particleInitialVelocityRotationRange.x*.5f,
						rand() % (static_cast<int>(this->particleSystemDefinition.particleInitialVelocityRotationRange.y)) - this->particleSystemDefinition.particleInitialVelocityRotationRange.y*.5f,
						rand() % (static_cast<int>(this->particleSystemDefinition.particleInitialVelocityRotationRange.z)) - this->particleSystemDefinition.particleInitialVelocityRotationRange.z*.5f,
						0), XMVectorReplicate(3.14159265359f / 180.f)));
					//			const XMVECTOR newVelocityRotation = this->particleSystemDefinition.particleEmissionVelocityRotation;

					const float initialSpeedDelta = this->particleSystemDefinition.particleInitialSpeedMax - this->particleSystemDefinition.particleInitialSpeedMin;
					const float initialSpeed = this->particleSystemDefinition.particleInitialSpeedMin + (rand()%100)*.01f*initialSpeedDelta;
					//XMVECTOR newVelocity = XMVector3Rotate(this->particleSystemDefinition.particleEmissionDirection[whichDirection],XMQuaternionRotationRollPitchYawFromVector(newVelocityRotation));
					//newVelocity = XMVector4Transform(newVelocity, this->transformation);
					XMVECTOR newVelocity = XMVector3Normalize(XMVector4Transform(this->particleSystemDefinition.particleEmissionDirection[whichDirection], this->transformation));
					newVelocity = XMVector3Rotate(newVelocity, XMQuaternionRotationRollPitchYawFromVector(newVelocityRotation));
					newVelocity = XMVectorMultiply(newVelocity, XMVector3Length(this->particleSystemDefinition.particleEmissionDirection[whichDirection]));
					this->particles[this->currentParticleCount].lifeTime = this->particleSystemDefinition.particleDuration;
					this->particles[this->currentParticleCount].location = XMVectorAdd(this->location, this->offset);
					this->particles[this->currentParticleCount].velocity = XMVectorAdd(this->velocity, XMVectorMultiply(newVelocity, XMVectorReplicate(initialSpeed)));

					this->particles[this->currentParticleCount].acceleration = XMVectorMultiply(
						this->particleSystemDefinition.particleAccelerationDirection,
						XMVectorReplicate(this->particleSystemDefinition.particleInitialAccelerationMagnitude));

					this->particles[this->currentParticleCount].tint = XMVectorSet(1, (rand()%40)*.01f + .25f, (rand()%10)*.005f, 1);//XMVectorSet(1, 1, 1, 1);//XMVectorSet(rand()%100 *.01f, rand()%100 *.01f, rand()%100 *.01f, 1);
					this->particles[this->currentParticleCount].active = true;
					this->particles[this->currentParticleCount].radius = this->particleSystemDefinition.particleRadius;

					++this->currentParticleCount;
				}
			}
		}
	}

	ParticleSystem::~ParticleSystem()
	{
		delete[] this->particles;
	}
}