#include "pch.h"
#include "TreeActor.h"
#include <random>
#include "../Common Source/LSystem/LSystem.h"
#include "../Common Source/LSystem/SpatialLSystemSegment.h"
#include "../Common Source/LSystem/SystemParameters.h"
#include "../Common Source/LSystem/TreeTrunkSegment.h"
#include "../Common Source/Terrain Simulation/TerrainResources.h"
#include "../Common Source/LSystem/SpatialLSystem.h"
#include "../../../Common Source/Terrain Simulation/GrowthSimulation.h"

using namespace DirectX;
using namespace Geometry;
using namespace WorldGrowthSimulation;

namespace SimulationEngine
{
	TreeActor::TreeActor(GrowthSimulation* const growthSimulation, const ObjectParameters& objectParameters,
		const MoveableParameters& moveableParameters,
		const ActorParameters& actorParameters) :
	Actor(objectParameters, moveableParameters, actorParameters),
		Growable(growthSimulation),
		treeSystem(new SpatialLSystem(growthSimulation->GetTerrainResources(), this))
	{
		SpatialLSystemSegment* rootNode;
 
		TrunkParameters trunkParameters = TrunkParameters(TreeBranchParameters(
				SystemParameters::MinimumBranchCount,
				SystemParameters::MaximumBranchCount,
				SystemParameters::BranchZMin,
				SystemParameters::BranchZMax,
				SystemParameters::MinimumYSeparation,
				SystemParameters::MaximumYSeparation,
				SystemParameters::BranchRadiusFactor,
				SystemParameters::BranchRadiusRange),

			SystemParameters::radiusDeltaFactorMin,
			SystemParameters::radiusFactorRange, 
			SystemParameters::lengthFactorMin, 
			SystemParameters::lengthFactorRange, 
			SystemParameters::trunkSegmentCount,
			SystemParameters::zRotationMin,
			SystemParameters::zRotationMax,
			SystemParameters::yRotationMin,
			SystemParameters::yRotationMax);


		const int radStart = .5; //SystemParameters::TrunkStartRadius
		const XMVECTOR endPointV = XMVector3Transform(XMVectorSet(0, 0, 0, 0), XMMatrixMultiply(XMMatrixMultiply(XMMatrixTranslation(0, SystemParameters::TrunkStartLength, 0), DirectX::XMMatrixIdentity()), DirectX::XMMatrixIdentity()));
		const XMMATRIX endTransformation = XMMatrixMultiply(XMMatrixMultiply(XMMatrixTranslation(0, SystemParameters::TrunkStartLength, 0), DirectX::XMMatrixIdentity()), DirectX::XMMatrixIdentity());
		rootNode = new TreeTrunkSegment(this->treeSystem, NULL, SystemParameters::UseFaceNormals ? 100 : 100, 0,
			M3SpatialLSystemParameters(
			DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), SystemParameters::TrunkStartLength, radStart, radStart),
			endPointV,
			endTransformation,
			trunkParameters);

		this->treeSystem->SetRootSegment(rootNode);

		this->scale = XMVectorSet(1, 1, 1, 1);

		this->componentFlags = 0;
		//this->componentFlags |= PhysicalWorld::ACT;
		//this->componentFlags |= PhysicalWorld::PATH;
		//this->componentFlags |= PhysicalWorld::PHYSICS;

		this->objectType = ObjectTypes::TreeActor;
		//this->locked = true;
	}

	bool TreeActor::Grow()
	{
		const bool updated = this->treeSystem->Update();
		if (updated)
		{
			++this->updateVersion;
		}

		static const int seedChance = 500;
		static const float maxDist = 300;
		static const float minDist = 30;

		if (rand() % seedChance == 5)
		{
			const float angle = (rand() % 360) * 3.1415 / 180;
			const float dist = (rand() % 100) * .01f * maxDist + minDist;

			const XMVECTOR offset = XMVector3Transform(XMVectorSet(0, 0, dist, 0), XMMatrixRotationY(angle));
			const XMVECTOR seedLoc = XMVectorAdd(this->location, offset);
			XMFLOAT4 seedLocF;
			XMStoreFloat4(&seedLocF, seedLoc);
			const WorldGrowthSimulation::TerrainResources::Voxel* const voxel = this->growthSimulation->GetTerrainResources()->GetVoxel(seedLocF.x, seedLocF.y, seedLocF.z);
			if (voxel == nullptr)
			{
				TreeActor* const newTree = new TreeActor(this->growthSimulation, ObjectParameters(this->GetParentWorld()), MoveableParameters(seedLoc), ActorParameters());
				//this->growthSimulation->GetTerrainResources()->AddVoxel(seedLocF.x, seedLocF.y, seedLocF.z, newTree->GetTreeSystem(), 1);				
			}
		}

		return updated;
	}

	void TreeActor::onRemove()
	{
		this->removeThisGrowable();
	}

	TreeActor::~TreeActor()
	{
		delete this->treeSystem;
	}
}