#include "pch.h"
#include "ModelNode.h"
#include "SceneEntity.h"

namespace RenderingEngine
{
	ModelNode::ModelNode() :
		sceneEntityCount(0)
	{

	}

	void ModelNode::AddSceneEntity(SceneEntity* sceneEntity)
	{
		if(this->sceneEntityCount >= this->sceneEntities.size())
		{
			this->sceneEntities.push_back(sceneEntity);
		}
		else
		{
			this->sceneEntities[this->sceneEntityCount] = sceneEntity;
		}

		sceneEntity->modelEntityID = this->sceneEntityCount;
		++this->sceneEntityCount;
	}

	void ModelNode::RemoveModelEntity(SceneEntity* sceneEntity)
	{
		if(!sceneEntity->removed)
		{
			sceneEntity->removed = true;
			this->sceneEntityRemoveList.push_back(sceneEntity);
		}
	}

	void ModelNode::DeleteRemovedModelEntities()
	{
		if(this->sceneEntityRemoveList.size() > 0)
		{
			for(unsigned int a = 0; a < this->sceneEntityRemoveList.size(); ++a)
			{
				--this->sceneEntityCount;
				SceneEntity* const tempEntity = this->sceneEntities[this->sceneEntityCount];
				this->sceneEntities[this->sceneEntityCount] = nullptr;
				if(this->sceneEntityRemoveList[a]->modelEntityID < this->sceneEntityCount)
				{
					this->sceneEntities[this->sceneEntityRemoveList[a]->modelEntityID] = tempEntity;
					this->sceneEntities[this->sceneEntityRemoveList[a]->modelEntityID]->modelEntityID = this->sceneEntityRemoveList[a]->modelEntityID;
				}

				delete this->sceneEntityRemoveList[a];
			}

			this->sceneEntityRemoveList.clear();
		}
	}

	ModelNode::~ModelNode()
	{
		for(int a = 0; a < this->sceneEntityCount; ++a)
		{
			delete this->sceneEntities[a];
		}
	}
}