#pragma once
#include "ObjectRendererBinding.h"
#include "..\Common Source\Simulation Engine\Physical Simulation\PhysicalWorldSimulation.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Base\ModelBufferData.h"

namespace Geometry { class LSystemMesh; class UniformHeightMapMesh; }
namespace ProceduralGeneration { class PerlinHeightMapGenerator; }

namespace InverseKinematics { class Skeleton; class SkeletonSegment; }


namespace RenderingEngine
{
	class SceneEntity;
	class SceneNode;

	struct SkeletonMeshData
	{
		std::vector<SceneNode*> segmentNodes;
	};

	struct LocalObjectData
	{
		unsigned int previousMeshVersion = 0;
		Geometry::LSystemMesh* mesh;
		SkeletonMeshData* skeletonMeshData = nullptr;
		~LocalObjectData()
		{
			if (this->skeletonMeshData != nullptr)
			{
				delete this->skeletonMeshData;
			}
		}
	};

	struct TerrainCellMesh
	{
		SceneEntity* sceneEntity;
		Geometry::UniformHeightMapMesh* mesh;
		ModelShaderData shaderData;
		_LARGE_INTEGER startTime;
		int cellX;
		int cellZ;
		bool isLodMesh = false;
		DirectX::XMVECTOR center;
		Model* model;
		long long key;
		unsigned int index;
	};

	struct TerrainCellGeneratorObject
	{
		float distToPlayer;
		std::function<void()> generationFunc;
		bool operator <(const TerrainCellGeneratorObject& rhs) const
		{
			return distToPlayer > rhs.distToPlayer;
		}
	};

	/*struct TerrainCellPositionComparator
	{
		const std::function<bool(const TerrainCellMesh* const, const TerrainCellMesh* const)> comp;
		TerrainCellPositionComparator(std::function<bool(const TerrainCellMesh* const, const TerrainCellMesh* const)> comp) : comp(comp) {}
		bool operator() (const TerrainCellMesh* const o1, const TerrainCellMesh* const o2) const;
	};*/
	//ActorRendererBindingGraph controls how the renderer retrieves newly-created objects
	//from the simulation engine and instantiates nodes for them in the renderer.
	class ActorRendererBindingGraph : public ObjectRendererBindingGraph<SimulationEngine::PhysicalWorldSimulation, SimulationEngine::Actor::ObjectData, FractalTreeRenderer, LocalObjectData>
	{
		typedef SimulationEngine::Actor::ObjectData ActorData;
		int clientID;
		SceneEntity* litNode;
		SceneNode* litNode2;
		SceneNode* skyNode = nullptr;
		SceneNode* waterNode = nullptr;
		SceneNode* voxelGridNode = nullptr;
		std::mutex terrainCellMutex;
		//std::priority_queue<TerrainCellGeneratorObject, std::vector<TerrainCellGeneratorObject>, std::function<bool(TerrainCellGeneratorObject, TerrainCellGeneratorObject)> > funcQueue;
		std::priority_queue<TerrainCellGeneratorObject> funcQueue;
		std::unordered_map<long long, TerrainCellMesh*> terrainCellNodes;
		std::vector<TerrainCellMesh*> terrainCellNodesList;
		std::queue<TerrainCellMesh*> completedTerrainCells;
		std::mutex terrainCellQueueMutex;
		unsigned int terrainCellNodeCount = 0;
		std::unordered_set<long long> seenVoxelKeys;

	public:
		ActorRendererBindingGraph(const SimulationEngine::PhysicalWorldSimulation* physicalWorldSimulation, SceneGraph* sceneGraph, const FractalTreeRenderer* renderer);
		void SetClientID(int clientID) {this->clientID = clientID;}
		~ActorRendererBindingGraph();

	protected:
		//Overload to specify creation of a scene node from a given object data.
		virtual SceneNode* CreateSceneNodeFromObjectData(const ActorData& objectData, LocalObjectData* localBindingData) override;

		//Overload to specify the update of a scene node from a given object data.
		virtual void UpdateSceneNodeFromObjectData(SceneNode* sceneNode, const ActorData& objectData, LocalObjectData* localBindingData) override;

		//Overload to specify the deletion of a scene node.
		virtual void OnDeleteSceneNodeFromObjectData(SceneNode* sceneNode, const ActorData& previousObjectData, LocalObjectData* localBindingData) override;

		//Overload to specify changes in the renderer based on the world state (for example: camera location).
		virtual void UpdateRendererProperties() override;

	private:
		SceneNode* createTreeActorSceneNode(const ActorData& objectData, LocalObjectData* localBindingData) const;
		SceneNode* createHumanoidSceneNode(const ActorData& objectData) const;
		SceneNode* createTestFloorSceneNode(const ActorData& objectData) const;
		SceneNode* createProjectileSceneNode(const ActorData& objectData);
		SceneNode* createTheSunSceneNode(const ActorData& objectData) const;
		SceneNode* createDecentMissile(const ActorData& objectData);
		SceneNode* createIneffectiveMissile(const ActorData& objectData);
		SceneNode* createBadGuySceneNode(const ActorData& objectData);
		SceneNode* createWorldSceneNode(const ActorData& objectData, LocalObjectData* localBindingData);
		SceneNode* createTestSkeletonUserSceneNode(const ActorData& objectData, LocalObjectData* const localBindingData);
		SceneNode* createTestChainLinkSceneNode(const ActorData& objectData, LocalObjectData* const localBindingData);
		SceneNode* createTestSkeletonArmSegmentSceneNode(const ActorData& objectData, LocalObjectData* const localBindingData);

		void updateTreeActorSceneNode(SceneNode* sceneNode, const ActorData& objectData, LocalObjectData* localBindingData) const;
		void updateTerrainSceneNode(SceneNode* sceneNode, const ActorData& objectData, LocalObjectData* localBindingData);
		void updateTestSkeletonUserSceneNode(SceneNode* const sceneNode, const ActorData& objectData, LocalObjectData* const localBindingData) const;

		void updateSkeleton(const ActorData & objectData, LocalObjectData * const localBindingData, const DirectX::XMVECTOR & dest, const InverseKinematics::Skeleton * const skeleton, SceneNode* rootNode);

		SceneNode * createDynamicSkeletonUserSceneNode(const ActorData & objectData, LocalObjectData * const localBindingData);

		void updateDynamicSkeletonUserSceneNode(SceneNode * const sceneNode, const ActorData & objectData, LocalObjectData * const localBindingData);
		
		TerrainCellMesh* queueTerrainCell(const int cellX,
			const int cellZ,
			const float cellWidth,
			const DirectX::XMFLOAT4& groundLoc,
			const ProceduralGeneration::PerlinHeightMapGenerator* const generator,
			const int cellToReplace); //-1 for none
		//add to only list
		void addTerrainCellNode(TerrainCellMesh* const cell);
		//remove from list and from map
		void removeTerrainCellNode(const unsigned int index, const bool beingReplaced);
		static void terrainWorkerThread(ActorRendererBindingGraph* bindingGraph);
		static inline long long getTerrainCellKey(const int cellX, const int cellZ) { return ((long long)cellX) * 2147483647L + (long long)cellZ;}
	};
}