#pragma once
#ifndef _WIN8_1
#ifndef _RTS_GAME
#include "BoundingBox.h"
#else
#include "..\Src\BoundingBox.h"
#endif
#else
#include "..\Src\BoundingBox.h"
#endif

namespace RenderingEngine
{
	class SceneNode;
	struct Model;

	class SceneEntity
	{
	public:
		SceneNode* parentNode;
		Model* model;
		int modelEntityID;
		bool removed;
		bool deleteModel;
		Geometry::BoundingBox boundingBox;
		bool useBoundingBox;
		bool enableZBuffer;

	public:
		SceneEntity(SceneNode* parentNode, RenderingEngine::Model* model, bool deleteModel = true, bool enableZBuffer = true);
		~SceneEntity(void);
		Model* GetModel() const {return this->model;}
	};
}

