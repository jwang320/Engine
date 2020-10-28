#pragma once
#include <vector>

namespace RenderingEngine
{
	class ParticleEntity;

	//Interface denoting a scene node that has a particle entity.  This interface
	//is meant to clean up the code in the Scene Node class.
	class ParticleNode
	{
	protected:
		std::vector<ParticleEntity*> particleEntities;
		int particleEntityCount;
		std::vector<ParticleEntity*> particleEntityRemoveList;

	public:
		void AddParticleSceneEntity(ParticleEntity* particleSceneEntity);
		ParticleEntity* GetParticleEntity(int index) const {return this->particleEntities[index];}
		unsigned int GetParticleEntityCount() const {return this->particleEntityCount;}
		void RemoveParticleEntity(ParticleEntity* particleEntity);

	protected:
		ParticleNode();
		void UpdateParticleNode();
		void DeleteRemovedParticleEntities();
		~ParticleNode();
	};
}