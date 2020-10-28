#include "pch.h"
#include "ObjectRendererBinding.h"
#include "SpaceGameRenderer.h"
#include "SpaceGameBindingGraph.h"
#include "..\SpaceGame Common\Core\SpaceGameSimulation.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\SceneGraph.h"


using namespace SimulationEngine;

namespace RenderingEngine
{
	template class ObjectRendererBindingGraph<SpaceGameCore::SpaceGameSimulation, Actor::ObjectData, SpaceGameCore::SpaceGameRenderer, SpaceGameCore::LocalObjectData>;

	template <class BoundContainerType, class BoundObjectDataType, class RendererType, class LocalNodeDataType>
	ObjectRendererBindingGraph<BoundContainerType, BoundObjectDataType, RendererType, LocalNodeDataType>::ObjectRendererBindingGraph(const BoundContainerType* worldSimulation, SceneGraph* sceneGraph, const RendererType* renderer) :
		worldSimulation(worldSimulation),
		renderer(renderer),
		sceneGraph(sceneGraph)
	{
		this->localBindingIDMap.resize(100000, -1);
	}

	template <class BoundContainerType, class BoundObjectDataType, class RendererType, class LocalNodeDataType>
	ObjectRendererBindingGraph<BoundContainerType, BoundObjectDataType, RendererType, LocalNodeDataType>::~ObjectRendererBindingGraph()
	{
		for(int a = 0; a < this->nextAvailableBindingID; ++a)
		{
			if (this->objectRendererBindings[a] != NULL)
				delete this->objectRendererBindings[a];
		}
	}

	template <class BoundContainerType, class BoundObjectDataType, class RendererType, class LocalNodeDataType>
	void ObjectRendererBindingGraph<BoundContainerType, BoundObjectDataType, RendererType, LocalNodeDataType>::Update()
	{
		if(this->worldSimulation == NULL)
		{
			return;
		}

		//Iterate through the list of bound nodes and check if the simulation
		//has deleted the object the node is bound to.
		for(int a = 0; a < this->nextAvailableBindingID; ++a)
		{
			if(this->worldSimulation->IsObjectValid(this->objectRendererBindings[a]->object->GetGlobalID()))
			{
				continue;
			}
			
			//Object is no longer valid;  delete.
			this->sceneGraph->RequestNodeRemoval(this->objectRendererBindings[a]->boundNode);
			this->OnDeleteSceneNodeFromObjectData(this->objectRendererBindings[a]->boundNode,
				this->objectRendererBindings[a]->objectData,
				this->objectRendererBindings[a]->nodeData);

			this->localBindingIDMap[this->objectRendererBindings[a]->object->GetGlobalID()] = -1;
			delete this->objectRendererBindings[a];

			if(a < this->nextAvailableBindingID - 1)
			{
				this->objectRendererBindings[a] = this->objectRendererBindings[this->nextAvailableBindingID - 1];
				this->objectRendererBindings[a]->localBindingID = a;
				this->localBindingIDMap[this->objectRendererBindings[a]->object->GetGlobalID()] = a;
				--a;
			}

			--this->nextAvailableBindingID;		
		}

		this->sceneGraph->DeleteRemovedNodes();

		this->UpdateRendererProperties();

		const unsigned int objectCount = this->worldSimulation->GetObjectCount();
		
		this->simulationObjectCount = 0;
		//Iterate through objectCount objects in the container to either bind to a scene node or (if already bound) update the objects.
		//At this point it's not necessary to know what the objects are.
		for(unsigned int a = 0; a < objectCount; ++a)
		{
			const int globalID = this->worldSimulation->GetObjectGlobalID(a);
			if(globalID < 0 )
			{
				//invalid or unusable object.
				continue; 
			}

			Object* object = this->worldSimulation->GetWorldObjectByGlobalID(globalID);
			if(object->GetGlobalID() < 0 || object->GetObjectType() == ObjectTypes::NOT_AN_OBJECT)
			{
				continue;
			}

			++this->simulationObjectCount;
			const int localBindingID = this->localBindingIDMap[globalID];

			//Check if there already exists a scene node binding for this object.
			if(localBindingID >= 0)
			{
				//Exists -- update the binding's object data
				this->objectRendererBindings[localBindingID]->objectData = this->worldSimulation->GetObjectData(globalID);

				this->UpdateSceneNodeFromObjectData(this->objectRendererBindings[localBindingID]->boundNode,
					this->objectRendererBindings[localBindingID]->objectData,
					this->objectRendererBindings[localBindingID]->nodeData);
			}
			else
			{
				object->Lock();

				//Object currently not bound; create new binding.
				ObjectRendererBinding* binding = new ObjectRendererBinding(object, NULL,
					this->worldSimulation->GetObjectData(globalID), new LocalNodeDataType());
				SceneNode* newSceneNode = this->CreateSceneNodeFromObjectData(binding->objectData, binding->nodeData);
				binding->boundNode = newSceneNode;
				this->AddBinding(binding);
			}
		}

		this->sceneGraph->Update();
	}

	template <class BoundContainerType, class BoundObjectDataType, class RendererType, class LocalNodeDataType>
	void ObjectRendererBindingGraph<BoundContainerType, BoundObjectDataType, RendererType, LocalNodeDataType>::AddBinding(ObjectRendererBinding* binding)
	{
		if(this->objectRendererBindings.size() > this->nextAvailableBindingID)
		{
			this->objectRendererBindings[this->nextAvailableBindingID] = binding;
		}
		else
		{
			this->objectRendererBindings.push_back(binding);
		}

		this->localBindingIDMap[binding->object->GetGlobalID()] = this->nextAvailableBindingID;
		++this->nextAvailableBindingID;
	}

	template <class BoundContainerType, class BoundObjectDataType, class RendererType, class LocalNodeDataType>
	ObjectRendererBindingGraph<BoundContainerType, BoundObjectDataType, RendererType, LocalNodeDataType>::ObjectRendererBinding::~ObjectRendererBinding()
	{
		this->object->Release();
		delete this->nodeData;
	}
}