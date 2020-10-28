#include "pch.h"
#include "ParticleNode.h"
#include "ParticleEntity.h"
namespace RenderingEngine
{
	ParticleNode::ParticleNode() :
		particleEntityCount(0)
	{

	}

	void ParticleNode::UpdateParticleNode()
	{
		for(int a = 0; a < this->particleEntityCount; ++a)
		{
			this->particleEntities[a]->Update();
		}
	}

	void ParticleNode::AddParticleSceneEntity(ParticleEntity* particleSceneEntity)
	{
		if(this->particleEntityCount >= this->particleEntities.size())
		{
			this->particleEntities.push_back(particleSceneEntity);
		}
		else
		{
			this->particleEntities[this->particleEntityCount] = particleSceneEntity;
		}

		particleSceneEntity->particleEntityID = this->particleEntityCount;
		++this->particleEntityCount;
	}

	void ParticleNode::RemoveParticleEntity(ParticleEntity* particleEntity)
	{
		if(!particleEntity->removed)
		{
			particleEntity->removed = true;
			this->particleEntityRemoveList.push_back(particleEntity);
		}
	}

	void ParticleNode::DeleteRemovedParticleEntities()
	{
		if(this->particleEntityRemoveList.size() > 0)
		{
			for(unsigned int a = 0; a < this->particleEntityRemoveList.size(); ++a)
			{
				--this->particleEntityCount;
				this->particleEntities[this->particleEntityCount] = NULL;
				if(this->particleEntityRemoveList[a]->particleEntityID < this->particleEntityCount)
				{
					this->particleEntities[this->particleEntityRemoveList[a]->particleEntityID] = this->particleEntities[this->particleEntityCount];
					this->particleEntities[this->particleEntityRemoveList[a]->particleEntityID]->particleEntityID = this->particleEntityRemoveList[a]->particleEntityID;
				}

				delete this->particleEntityRemoveList[a];
			}

			this->particleEntityRemoveList.clear();
		}
	}

	ParticleNode::~ParticleNode()
	{
		for(int a = 0; a < particleEntityCount; ++a)
		{
			delete this->particleEntities[a];
		}
	}
}