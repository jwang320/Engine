#pragma once
#include <vector>

namespace RenderingEngine
{
	class SceneEntity;

	//interface denoting a scene node with model entities.  this interface
	//is meant to help keep the SceneNode class clean.
	class ModelNode
	{
	protected:
		std::vector<SceneEntity*> sceneEntities;
		int sceneEntityCount;
		std::vector<SceneEntity*> sceneEntityRemoveList;

	public:
		void AddSceneEntity(SceneEntity* sceneEntity);
		unsigned int GetSceneEntityCount() const {return this->sceneEntityCount;}
		SceneEntity* GetSceneEntity(int index) const {return this->sceneEntities[index];}
		void RemoveModelEntity(SceneEntity* sceneEntity);
		//const std::vector<SceneEntity*>& GetSceneEntities() const {return this->sceneEntities;}

	protected:
		ModelNode();
		~ModelNode();

		void DeleteRemovedModelEntities();
	};
}