#pragma once
#include "ObjectRendererBinding.h"
#include "SpaceGameRenderer.h"
#include "..\SpaceGame Common\Core\SpaceGameSimulation.h"

namespace SpaceGameCore
{
	struct LocalObjectData
	{

	};

	//struct RTSBindingGraph::LocalObjectData;

	class SpaceGameBindingGraph : public RenderingEngine::ObjectRendererBindingGraph<SpaceGameSimulation, SimulationEngine::Actor::ObjectData, SpaceGameRenderer, LocalObjectData>
	{
		typedef SimulationEngine::Actor::ObjectData ActorData;
	private:
		int clientID;
		RenderingEngine::SceneNode* mouseNode = nullptr;

	public:
		SpaceGameBindingGraph(const SpaceGameSimulation* physicalWorldSimulation, RenderingEngine::SceneGraph* sceneGraph, const SpaceGameRenderer* renderer);

		//Overload to specify creation of a scene node from a given object data.
		virtual RenderingEngine::SceneNode* CreateSceneNodeFromObjectData(const SimulationEngine::Actor::ObjectData& objectData, LocalObjectData* localBindingData) override;

		//Overload to specify the update of a scene node from a given object data.
		virtual void UpdateSceneNodeFromObjectData(RenderingEngine::SceneNode* sceneNode, const SimulationEngine::Actor::ObjectData& objectData, LocalObjectData* localBindingData) override;

		//Overload to specify the deletion of a scene node.
		virtual void OnDeleteSceneNodeFromObjectData(RenderingEngine::SceneNode* sceneNode, const SimulationEngine::Actor::ObjectData& previousObjectData, LocalObjectData* localBindingData) override;

		//Overload to specify changes in the renderer based on the world state (for example: camera location).
		virtual void UpdateRendererProperties() override;


		void SetClientID(int clientID) { this->clientID = clientID; }
		~SpaceGameBindingGraph();
	private:
		void updatePeasantSceneNode(RenderingEngine::SceneNode* sceneNode, const ActorData& objectData, LocalObjectData* localBindingData);
		RenderingEngine::SceneNode* createPeasantSceneNode(const ActorData& objectData);
	};
}