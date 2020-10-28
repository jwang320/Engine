#pragma once
#include "..\Src\ObjectRendererBinding.h"
#include "GameSimulation.h"
#include "GameRenderer.h"
#include "..\Rendering Engine\ModelBufferData.h"

namespace Win8GameRenderingEngine
{
	struct LocalObjectData
	{
		bool constructionComplete;
		unsigned int textureRow;
		unsigned int* textureColor;
		unsigned int textureSize;
		DirectX::XMVECTOR previousQueriedLocation;
		unsigned int previousCompletedCount;
		unsigned int previousMeshVersion;
		Geometry::Mesh* mesh;
		bool hasGeneratedMesh;
		bool generationBegun;
		RenderingEngine::ModelShaderData shaderData;
		bool isLodMesh;
		bool nodeOffset;
		int lastAnimationStageIndex;
		LocalObjectData() :
			constructionComplete(false),
			hasGeneratedMesh(false),
			generationBegun(false),
			isLodMesh(false),
			textureRow(0),
			textureColor(NULL),
			textureSize(0),
			previousCompletedCount(0),
			previousMeshVersion(0),
			nodeOffset(0),
			lastAnimationStageIndex(0)
		{

		}

		~LocalObjectData()
		{
			if (this->textureColor != NULL)
			{
				delete[] this->textureColor;
			}

		}
	};

	class GameBindingGraph : public RenderingEngine::ObjectRendererBindingGraph<GameSimulation, SimulationEngine::Actor::ObjectData, GameRenderer, LocalObjectData>
	{
		typedef SimulationEngine::Actor::ObjectData ActorData;
	public:

	protected:
		int clientID;
		DirectX::XMFLOAT4 mouseLoc;
		RenderingEngine::SceneNode* addBlockSceneNode;
		int playerCharacterID;
		RenderingEngine::SceneNode* light1Node;
		unsigned int frameCounter;

	public:
		GameBindingGraph(const GameSimulation* physicalWorldSimulation, RenderingEngine::SceneGraph* sceneGraph, const GameRenderer* renderer);
		void SetClientID(int clientID) { this->clientID = clientID; }
		void SetMouseLocation(float x, float y) {this->mouseLoc.x = x; this->mouseLoc.y = 0;}
	protected:
		//Overload to specify creation of a scene node from a given object data.
		virtual RenderingEngine::SceneNode* CreateSceneNodeFromObjectData(const ActorData& objectData, LocalObjectData* localBindingData) override;

		//Overload to specify the update of a scene node from a given object data.
		virtual void UpdateSceneNodeFromObjectData(RenderingEngine::SceneNode* sceneNode, const ActorData& objectData, LocalObjectData* localBindingData) override;

		//Overload to specify the deletion of a scene node.
		virtual void OnDeleteSceneNodeFromObjectData(RenderingEngine::SceneNode* sceneNode, const ActorData& previousObjectData, LocalObjectData* localBindingData) override;

		//Overload to specify changes in the renderer based on the world state (for example: camera location).
		virtual void UpdateRendererProperties() override;

	private:
		RenderingEngine::SceneNode* createBlockUniverseSceneNode(const ActorData& objectData, LocalObjectData* localBindingData);
		RenderingEngine::SceneNode* createPlatformerCharacterSceneNode(const ActorData& objectData, LocalObjectData* localBindingData, bool createCrosshair = false);
		RenderingEngine::SceneNode* createPlatformerPersonCharacterSceneNode(const ActorData& objectData, LocalObjectData* localBindingData);
		RenderingEngine::SceneNode* createPlatformerMissileSceneNode(const ActorData& objectData, LocalObjectData* localBindingData);

		void updatePlatformerCharacterSceneNode(RenderingEngine::SceneNode* sceneNode, const ActorData& objectData, LocalObjectData* localBindingData);
		void updatePlatformerPersonCharacterSceneNode(RenderingEngine::SceneNode* sceneNode, const ActorData& objectData, LocalObjectData* localBindingData);
	};
}