#include "pch.h"
#include "ParticleEntity.h"
#include "ParticleSystem.h"
#include "SceneNode.h"
#include "..\Rendering Engine Base\ModelBufferData.h"

using namespace DirectX;

namespace RenderingEngine
{
	ParticleEntity::ParticleEntity(SceneNode* parentNode, RenderingEngine::Model* model, ParticleSystem* particleSystem, bool deleteModel) :
		parentNode(parentNode),
		model(model),
		particleSystem(particleSystem),
		removed(false),
		deleteModel(deleteModel)
	{

	}

	void ParticleEntity::Update()
	{
		this->particleSystem->location = XMVector4Transform(this->particleSystem->offset, this->parentNode->GetAbsoluteTransformation());
		this->particleSystem->transformation = this->parentNode->GetAbsoluteTransformation();
//		this->particleSystem->particleSystemRotationOffset = this->parentNode->GetPitchRollYaw();
		this->particleSystem->Update();
	}

	ParticleEntity::~ParticleEntity()
	{
		delete this->particleSystem;
		if(this->deleteModel)
		{
			delete this->model;
		}
	}
}