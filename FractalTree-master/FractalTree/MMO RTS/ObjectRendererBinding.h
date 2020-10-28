#pragma once
#include <vector>
namespace SimulationEngine {class Object;}

namespace RenderingEngine
{
	class SceneGraph;
	class SceneNode;
	//A class for binding a container of objects to a scene graph.
	//Assumes the container type has:
	//      int   GetObjectGlobalID(int objectIndex)
	//		_____ GetObjectData(int globalObjectID)

	//There are no restrictions on what BoundObjectDataType can have.
	template <class BoundContainerType, class BoundObjectDataType, class RendererType, class LocalNodeDataType>
	class ObjectRendererBindingGraph 
	{
	private:
		struct ObjectRendererBinding
		{
			SimulationEngine::Object* object;
			SceneNode* boundNode;
			int localBindingID;
			BoundObjectDataType objectData;
			LocalNodeDataType* nodeData;

			ObjectRendererBinding(SimulationEngine::Object* object, SceneNode* boundNode, const BoundObjectDataType& objectData, LocalNodeDataType* nodeData) :
				object(object),
				boundNode(boundNode),
				objectData(objectData),
				nodeData(nodeData)
			{

			}
			~ObjectRendererBinding();
		};

	private:
		std::vector<int> localBindingIDMap;
		int nextAvailableBindingID = 0;

	protected:
		std::vector<ObjectRendererBinding*> objectRendererBindings;
		SceneGraph* sceneGraph;
		const BoundContainerType* worldSimulation;
		const RendererType* renderer;
		int simulationObjectCount = 0;

	public:
		ObjectRendererBindingGraph(const BoundContainerType* worldSimulation, SceneGraph* sceneGraph, const RendererType* renderer);
		void Update();
		virtual ~ObjectRendererBindingGraph();

	protected:
		//Overload to specify creation of a scene node from a given object data.
		virtual SceneNode* CreateSceneNodeFromObjectData(const BoundObjectDataType& objectData, LocalNodeDataType* localBindingData) = 0;

		//Overload to specify the update of a scene node from a given object data.
		virtual void UpdateSceneNodeFromObjectData(SceneNode* sceneNode, const BoundObjectDataType& newObjectData, LocalNodeDataType* localBindingData) = 0;
		
		//Overload to specify how a node is deleted when its object is deleted
		virtual void OnDeleteSceneNodeFromObjectData(SceneNode* sceneNode, const BoundObjectDataType& previousObjectData, LocalNodeDataType* localBindingData) = 0;

		//Overload to specify changes in renderer based on world state (such as camera location).
		virtual void UpdateRendererProperties() = 0;

		//Runs when the renderer is first created and linked to the simulation.
	//	virtual void InitializeRendererProperties() = 0;
	private:
		void AddBinding(ObjectRendererBinding* binding);
	};
}