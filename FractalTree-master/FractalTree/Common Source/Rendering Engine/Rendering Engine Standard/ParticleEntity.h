#pragma once
#include "SceneEntity.h"

namespace RenderingEngine
{
	struct Model;
	class ParticleSystem;

	class ParticleEntity
	{
	public:
		SceneNode* parentNode;
		Model* model;
		ParticleSystem* particleSystem;
		int particleEntityID;
		bool removed;
		bool deleteModel;

	public:
		ParticleEntity(SceneNode* parentNode, RenderingEngine::Model* model, ParticleSystem* particleSystem, bool deleteModel = true);
		void Update();
		~ParticleEntity();
	};
}