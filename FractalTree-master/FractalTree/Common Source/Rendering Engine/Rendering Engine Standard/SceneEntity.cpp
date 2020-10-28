#include "pch.h"
#include "SceneEntity.h"
#include "..\Rendering Engine Base\ModelBufferData.h"

namespace RenderingEngine
{
	SceneEntity::SceneEntity(SceneNode* parentNode, RenderingEngine::Model* model, bool deleteModel, bool enableZBuffer) :
		parentNode(parentNode),
		model(model),
		removed(false),
		deleteModel(deleteModel),
		useBoundingBox(false),
		enableZBuffer(enableZBuffer)

	{

	}

	SceneEntity::~SceneEntity(void)
	{
		if(this->deleteModel)
		{
			delete this->model;
		}
	}
}
