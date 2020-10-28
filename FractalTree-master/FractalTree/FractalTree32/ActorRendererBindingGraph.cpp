#include "pch.h"
#include "ActorRendererBindingGraph.h"
#include "FractalTreeRenderer.h"
#include "UIRenderer.h"
#include "..\Common Source\Skeleton\Skeleton.h"
#include "..\Common Source\Skeleton\SkeletonSegment.h"
#include "..\FractalTree Common\Game Objects\Objects\DynamicUserOfSkeleton.h"

using namespace SimulationEngine;
using namespace DirectX;
using namespace Geometry;
using namespace std;

extern float camHeight;
int foliageTimeInd;
int foliageIterateTimeInd;
int treeVertexInd;
int treeIndexInd;
static int generationTimeInd;
static bool showCount = true;
extern double performanceFrequency;
static int queueDebugIndex = 0;
static int cellCountIndex = 0;
static int objectCountIndex = 0;

using namespace InverseKinematics;

namespace RenderingEngine
{
	float ang2 = 0;

	ActorRendererBindingGraph::ActorRendererBindingGraph(const PhysicalWorldSimulation* physicalWorldSimulation, SceneGraph* sceneGraph, const FractalTreeRenderer* renderer) :
		ObjectRendererBindingGraph<PhysicalWorldSimulation, ActorData, FractalTreeRenderer, LocalObjectData>(physicalWorldSimulation, sceneGraph, renderer),
		clientID(0),
		litNode(nullptr)
	{
		if (physicalWorldSimulation->generateTerrain)
		generationTimeInd = UIRenderer::AddDebugString("Cell generation time: ");
		objectCountIndex = UIRenderer::AddDebugString("Object count: ");

		for (int a = 0; a < FractalTreeResources::terrainThreadCount; ++a)
		{
			using namespace std;
			thread workerThread = thread(&terrainWorkerThread, this);
			workerThread.detach();
		}

		if (FractalTreeResources::useSky)
		{
			this->skyNode = new SceneNode(this->sceneGraph);
			Model* const newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["skydome.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SkyVertexShader, FractalTreeRenderer::SkyPixelShader, FractalTreeRenderer::SkyInputLayout));
			newModel->deleteBuffer = false;
			SceneEntity* const newEntity = new SceneEntity(skyNode, newModel);
			newEntity->enableZBuffer = false;
			skyNode->AddSceneEntity(newEntity);
		}
		if (FractalTreeResources::useWater)
		{
			this->waterNode = new SceneNode(this->sceneGraph);
			Model* const newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::WaterPixelShader, FractalTreeRenderer::DefaultInputLayout));
			newModel->deleteBuffer = false;
			SceneEntity* const newEntity = new SceneEntity(waterNode, newModel);
			newModel->diffuseTint = XMVectorSet(.07f, .2f, .75f, .85f);
			//newEntity->enableZBuffer = false;
			waterNode->AddSceneEntity(newEntity);
			waterNode->SetTransformationVector(XMVectorSet(1000000, 1, 1000000, 1), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, -600, 0, 1));
		}


#ifndef _NO_GRAPHICS
		if (physicalWorldSimulation->generateTerrain)
		{
			queueDebugIndex = UIRenderer::AddDebugString("Cells Queued: ");
			cellCountIndex = UIRenderer::AddDebugString("Cell Count: ");
		}
#endif
	}

	SceneNode* ActorRendererBindingGraph::CreateSceneNodeFromObjectData(const ActorData& objectData, LocalObjectData* localBindingData)
	{
		switch(objectData.objectType)
		{
		case ObjectTypes::TreeActor:
			{
				return this->createTreeActorSceneNode(objectData, localBindingData);
			}
		case ObjectTypes::Humanoid:
			{
				return this->createHumanoidSceneNode(objectData);
			}
		case ObjectTypes::GoodGuy:
			{
				return this->createBadGuySceneNode(objectData);
			}
		case ObjectTypes::TestFloor:
			{
				return this->createTestFloorSceneNode(objectData);
			}
		case ObjectTypes::Projectile:
			{
				return this->createProjectileSceneNode(objectData);
			}
		case ObjectTypes::TheSun:
			{
				return this->createTheSunSceneNode(objectData);
			}
		case ObjectTypes::DecentMissile:
			{
				return this->createDecentMissile(objectData);
			}
		case ObjectTypes::IneffectiveMissile:
			{
				return this->createIneffectiveMissile(objectData);
			}
		case ObjectTypes::BadGuy:
			{
				return this->createBadGuySceneNode(objectData);
			}
		case ObjectTypes::Terrain:
			{
				return this->createWorldSceneNode(objectData, localBindingData);
			}
		case ObjectTypes::TestUserOfSkeleton:
		{
			return this->createTestSkeletonUserSceneNode(objectData, localBindingData);
		}
		case ObjectTypes::DynamicSkeleton:
		{
			return this->createDynamicSkeletonUserSceneNode(objectData, localBindingData);
		}
		case ObjectTypes::TestSkeletonArmSegment:
		{
			return this->createTestSkeletonArmSegmentSceneNode(objectData, localBindingData);
		}
		case ObjectTypes::TestChainLink:
		{
			return this->createTestChainLinkSceneNode(objectData, localBindingData);
		}
		default:
			{
				SceneNode* newNode = new SceneNode(this->sceneGraph);
				newNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);
				return newNode;
			}
		}	
	}

	bool setPos = true;
	void ActorRendererBindingGraph::UpdateSceneNodeFromObjectData(SceneNode* sceneNode, const ActorData& objectData, LocalObjectData* localBindingData)
	{
		sceneNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);
		sceneNode->SetVelocity(objectData.object->velocity);

		switch (objectData.objectType)
		{
		case ObjectTypes::TreeActor:
		{
			this->updateTreeActorSceneNode(sceneNode, objectData, localBindingData);
		}
		break;
		case ObjectTypes::Projectile:
		{
			if (sceneNode == this->litNode2)
			{
				this->sceneGraph->SetLightPosition(XMVectorAdd(objectData.location, XMVectorSet(0, 50, 0, 0)));
				setPos = false;
			}
		}
		break;
		case ObjectTypes::TheSun:
		{
			this->sceneGraph->SetSunPosition(objectData.location);
		}
		break;
		case ObjectTypes::TestUserOfSkeleton:
		{
			this->updateTestSkeletonUserSceneNode(sceneNode, objectData, localBindingData);
		}
		break;
		case ObjectTypes::DynamicSkeleton:
		{
			this->updateDynamicSkeletonUserSceneNode(sceneNode, objectData, localBindingData);
		}
		break;
		case ObjectTypes::BadGuy:
		{
			for (int a = 0; a < objectData.object->GetThrusters().size(); ++a)
			{
				SceneNode* const bottomLeftNode = sceneNode->GetChildByName(std::to_string(a));
				if (bottomLeftNode != NULL && bottomLeftNode->GetParticleEntityCount() > 0)
				{
					ParticleEntity* const particleEntity = bottomLeftNode->GetParticleEntity(0);
					if (particleEntity != NULL)
					{
						//const BadGuy::SpecialBadGuyData* badGuyData = static_cast<const BadGuy::SpecialBadGuyData*>(objectData.specialData);

						if (((objectData.object->GetThrusters()))[a]->activated)
						{
							particleEntity->particleSystem->enabled = true;
							particleEntity->particleSystem->velocity = objectData.object->velocity;
						}
						else
						{
							particleEntity->particleSystem->enabled = false;
						}
					}
				}
			}
		}
		break;
		case ObjectTypes::GoodGuy:
		{
			for (int a = 0; a < 15; ++a)
			{
				SceneNode* bottomLeftNode = sceneNode->GetChildByName(std::to_string(a));
				if (bottomLeftNode != NULL && bottomLeftNode->GetParticleEntityCount() > 0)
				{
					ParticleEntity* particleEntity = bottomLeftNode->GetParticleEntity(0);
					if (particleEntity != NULL)
					{
						//const BadGuy::SpecialBadGuyData* badGuyData = static_cast<const BadGuy::SpecialBadGuyData*>(objectData.specialData);

						if (((objectData.object->GetThrusters()))[a]->activated/* || a == GoodGuy::FRONT_LEFT || a == GoodGuy::BACK_RIGHT ||
																			   a == GoodGuy::TOP_BACK || a == GoodGuy::BOTTOM_FRONT*/)
						{
							particleEntity->particleSystem->enabled = true;
							particleEntity->particleSystem->velocity = objectData.object->velocity;
						}
						else
						{
							particleEntity->particleSystem->enabled = false;
						}
					}
				}
			}
			sceneNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, XMVectorSet(0, 0, 0, 1));
		}
		break;
		case ObjectTypes::Terrain:
		{
			this->updateTerrainSceneNode(sceneNode, objectData, localBindingData);
		}
		break;
		}

		if (sceneNode == this->litNode2)
		{
			this->sceneGraph->SetLightPosition(XMVectorAdd(objectData.location, XMVectorSet(0, 0, 0, 0)));
			setPos = false;
		}
	}

	SceneNode* ActorRendererBindingGraph::createWorldSceneNode(const ActorData& objectData, LocalObjectData* localBindingData)
	{
		SceneNode* const newNode = new SceneNode(this->sceneGraph);
		return newNode;
		if(FractalTreeResources::useSky)
		{
			Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["skydome.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SkyVertexShader, FractalTreeRenderer::SkyPixelShader, FractalTreeRenderer::SkyInputLayout));
			newModel->deleteBuffer = false;
			SceneEntity* newEntity = new SceneEntity(newNode, newModel);
			newEntity->enableZBuffer = false;
			newNode->AddSceneEntity(newEntity);
		}
		return newNode;
	}

	

	SceneNode* ActorRendererBindingGraph::createTreeActorSceneNode(const ActorData& objectData, LocalObjectData* localBindingData) const
	{
		SceneNode* const newNode = new SceneNode(this->sceneGraph);
		//return newNode;
		Model* newModel = nullptr;
		/*const LSystemMesh* const mesh = objectData.objectType == ObjectTypes::TreeActor ?
			static_cast<const TreeActor*>(objectData.object)->GetMesh() : static_cast<const FoliageActor*>(objectData.object)->GetMesh();*/
		if(FractalTreeResources::useLineMeshes)
		{
			newModel = this->renderer->CreateStandardModel<VertexColor>(FractalTreeRenderer::WireFrameVertexShader,
				FractalTreeRenderer::WireFramePixelShader,
				FractalTreeRenderer::WireFrameInputLayout,
				1100,
				1100);
			
			newModel->modelShaderData.primitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			newModel->isLineMesh = true;
		}
		else
		{
			newModel = this->renderer->CreateStandardModel<TexturedVertex>(FractalTreeRenderer::TerrainVertexShader,
				FractalTreeRenderer::TreePixelShader,
				FractalTreeRenderer::TerrainInputLayout,
				FractalTreeResources::maxTreeVertexCount,
				FractalTreeResources::maxTreeIndexCount);

			newModel->modelShaderData.primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			newModel->castsShadow = true;
		}

		newModel->deleteBuffer = true;
		SceneEntity* newEntity = new SceneEntity(newNode, newModel);
		newModel->modelShaderData.samplerStateSlot = 1;
		newModel->modelShaderData.resourceViewSlot = 1;
		newModel->modelShaderData.textureSamplerState = this->renderer->GetSamplerState(FractalTreeRenderer::TerrainSamplerState);
		if(SystemParameters::UseFaceNormals)
		{
			newModel->modelShaderData.textureResourceView = this->renderer->GetShaderResourceView(FractalTreeRenderer::MountainResourceView + 3);
		}
		else
		{
			newModel->modelShaderData.textureResourceView = this->renderer->GetShaderResourceView(FractalTreeRenderer::MountainResourceView);
		}

		newNode->AddSceneEntity(newEntity);
		newNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);
		localBindingData->previousMeshVersion = 0;
		localBindingData->mesh = nullptr;
		return newNode;
	}

	SceneNode* ActorRendererBindingGraph::createHumanoidSceneNode(const ActorData& objectData) const
	{
		SceneNode* newNode = new SceneNode(this->sceneGraph);
		Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["Pyramid2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		newModel->deleteBuffer = false;
		SceneEntity* newEntity = new SceneEntity(newNode, newModel);
		newNode->AddSceneEntity(newEntity);
		newNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);
		newModel->tint = XMVectorSet(.4f, 0, 0, 0);

		return newNode;
	}

	SceneNode* ActorRendererBindingGraph::createTestFloorSceneNode(const ActorData& objectData) const
	{
		SceneNode* const newNode = new SceneNode(this->sceneGraph);

		{
			SceneNode* const floorNode = new SceneNode(this->sceneGraph);
			Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
			newModel->deleteBuffer = false;
			SceneEntity* newEntity = new SceneEntity(floorNode, newModel);
			floorNode->AddSceneEntity(newEntity);
			floorNode->SetTransformationVector(XMVectorSet(8192, 1, 128192, 0), objectData.rollPitchYaw, objectData.location);
			newNode->AddChildNode(floorNode);
		}

		{
			SceneNode* const floorNode = new SceneNode(this->sceneGraph);
			Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
			newModel->deleteBuffer = false;
			SceneEntity* newEntity = new SceneEntity(floorNode, newModel);
			floorNode->AddSceneEntity(newEntity);
			floorNode->SetTransformationVector(XMVectorSet(1, 8192, 128192, 0), objectData.rollPitchYaw, XMVectorSet(-2000, 0, 0, 1));
			newNode->AddChildNode(floorNode);
		}

		{
			SceneNode* const floorNode = new SceneNode(this->sceneGraph);
			Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
			newModel->deleteBuffer = false;
			SceneEntity* newEntity = new SceneEntity(floorNode, newModel);
			floorNode->AddSceneEntity(newEntity);
			floorNode->SetTransformationVector(XMVectorSet(1, 8192, 128192, 0), objectData.rollPitchYaw, XMVectorSet(2000, 0, 0, 1));
			newNode->AddChildNode(floorNode);
		}

		{
			SceneNode* const floorNode = new SceneNode(this->sceneGraph);
			Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
			newModel->deleteBuffer = false;
			SceneEntity* newEntity = new SceneEntity(floorNode, newModel);
			floorNode->AddSceneEntity(newEntity);
			floorNode->SetTransformationVector(XMVectorSet(8192, 1, 128192, 0), objectData.rollPitchYaw, XMVectorSet(0, 4000, 0, 1));
			newNode->AddChildNode(floorNode);
		}

		{
			SceneNode* const floorNode = new SceneNode(this->sceneGraph);
			Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
			newModel->deleteBuffer = false;
			SceneEntity* newEntity = new SceneEntity(floorNode, newModel);
			floorNode->AddSceneEntity(newEntity);
			floorNode->SetTransformationVector(XMVectorSet(8192, 8192, 1, 0), objectData.rollPitchYaw, XMVectorSet(0, 0, 8000, 1));
			newNode->AddChildNode(floorNode);
		}

		{
			const float minX = -100;
			const float maxX = 100;
			const float minY = -500;
			const float maxY = 500;
			const float minZ = -100;
			const float maxZ = 100;
			const float yLoc = (maxY - minY) / 2 + minY;
			XMVECTOR loc = XMVectorSet(0, PhysicalWorld::moveY, 0, 1);
			XMVECTOR sc = XMVectorSet(200, 200, 200, 0);
			SceneNode* const floorNode = new SceneNode(this->sceneGraph);
			Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
			newModel->deleteBuffer = false;
			SceneEntity* newEntity = new SceneEntity(floorNode, newModel);
			floorNode->AddSceneEntity(newEntity);
			floorNode->SetTransformationVector(sc, XMQuaternionRotationRollPitchYawFromVector(XMVectorSet(PhysicalWorld::rotAngX, PhysicalWorld::rotAngY, PhysicalWorld::rotAngZ, 0)), loc);
			//floorNode->setro
			newNode->AddChildNode(floorNode);
		}

		return newNode;
	}

	SceneNode* ActorRendererBindingGraph::createProjectileSceneNode(const ActorData& objectData)
	{
		SceneNode* newNode = new SceneNode(this->sceneGraph);
		Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["sphere.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		newModel->deleteBuffer = false;
		SceneEntity* newEntity = new SceneEntity(newNode, newModel);
		newNode->AddSceneEntity(newEntity);
		newNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);

		if(this->litNode != NULL)
		{
			this->litNode->model->tint = XMVectorSet(0, 0, 0, 1);
			this->litNode->model->diffuseTint = XMVectorSet(1, 1, 1, 1);
		}
		this->litNode = newEntity;
		this->litNode2 = newNode;
		newModel->tint = XMVectorSet(.65f, 1, 0, 1);
		newModel->diffuseTint = XMVectorSet(0, 0, 0, 1);
		
		SceneNode* particleNode = new SceneNode(this->sceneGraph, true, 40);
		newNode->AddChildNode(particleNode);

		ParticleSystem* testParticleSystem = new ParticleSystem(XMVectorSet(0, 0, 0, 1), 
			FractalTreeResources::ParticleSystemDefinitions[FractalTreeResources::GrenadeFireTrail]);
		testParticleSystem->useQuads = true;
		ParticleEntity* testParticleEntity = new ParticleEntity(particleNode, this->renderer->CreateStandardParticleSystemModel(testParticleSystem->particleSystemDefinition.maxConcurrentParticles), testParticleSystem);
		particleNode->AddParticleSceneEntity(testParticleEntity);


		return newNode;
	}

	SceneNode* ActorRendererBindingGraph::createTheSunSceneNode(const ActorData& objectData) const
	{
		SceneNode* newNode = new SceneNode(this->sceneGraph);
		Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["sphere.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		newModel->deleteBuffer = false;
		SceneEntity* newEntity = new SceneEntity(newNode, newModel);
		newNode->AddSceneEntity(newEntity);
		newNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);
		newModel->tint = XMVectorSet(1, 1, 0, 0);
		return newNode;
	}

	SceneNode* ActorRendererBindingGraph::createDecentMissile(const ActorData& objectData)
	{
		SceneNode* newNode = new SceneNode(this->sceneGraph);
		Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["sphere.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		newModel->deleteBuffer = false;
		SceneEntity* newEntity = new SceneEntity(newNode, newModel);
		newNode->AddSceneEntity(newEntity);
		newNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);

		if(this->litNode != NULL)
		{
			this->litNode->model->tint = XMVectorSet(0, 0, 0, 0);
		}
		this->litNode = newEntity;
		this->litNode2 = newNode;
		newModel->tint = XMVectorSet(.65f, 1, 0, 0);

		SceneNode* particleNode = new SceneNode(this->sceneGraph);
		newNode->AddChildNode(particleNode);

		ParticleSystem* testParticleSystem = new ParticleSystem(XMVectorSet(0, 0, 0, 1), 
			FractalTreeResources::ParticleSystemDefinitions[FractalTreeResources::HitExplosion]);
		testParticleSystem->useQuads = true;
		ParticleEntity* testParticleEntity = new ParticleEntity(particleNode, this->renderer->CreateStandardParticleSystemModel(testParticleSystem->particleSystemDefinition.maxConcurrentParticles), testParticleSystem);
		particleNode->AddParticleSceneEntity(testParticleEntity);

		return newNode;
	}

	SceneNode* ActorRendererBindingGraph::createIneffectiveMissile(const ActorData& objectData)
	{
		SceneNode* newNode = new SceneNode(this->sceneGraph);
		Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["sphere.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		newModel->deleteBuffer = false;
		SceneEntity* newEntity = new SceneEntity(newNode, newModel);
		newNode->AddSceneEntity(newEntity);
		newNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);

		if(this->litNode != NULL)
		{
			this->litNode->model->tint = XMVectorSet(0, 0, 0, 0);
		}
		this->litNode = newEntity;
		this->litNode2 = newNode;

		SceneNode* particleNode = new SceneNode(this->sceneGraph);
		newNode->AddChildNode(particleNode);

		ParticleSystem* testParticleSystem = new ParticleSystem(XMVectorSet(0, 0, 0, 1), 
			FractalTreeResources::ParticleSystemDefinitions[FractalTreeResources::IneffectiveMissileFireTrail]);
		testParticleSystem->useQuads = true;
		ParticleEntity* testParticleEntity = new ParticleEntity(particleNode, this->renderer->CreateStandardParticleSystemModel(testParticleSystem->particleSystemDefinition.maxConcurrentParticles), testParticleSystem);
		particleNode->AddParticleSceneEntity(testParticleEntity);


		return newNode;
	}

	SceneNode* ActorRendererBindingGraph::createBadGuySceneNode(const ActorData& objectData)
	{
		const Actor* const obj = static_cast<const Actor*>(objectData.object);
		SceneNode* newNode = new SceneNode(this->sceneGraph);
		//Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["testship4.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["testship4.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		newModel->deleteBuffer = false;
		SceneEntity* newEntity = new SceneEntity(newNode, newModel);
		newNode->AddSceneEntity(newEntity);
		newNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);

		/*SceneNode* boxNode = new SceneNode(this->sceneGraph);
		//Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["testship4.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		Model* newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		newModel->deleteBuffer = false;
		SceneEntity* newEntity = new SceneEntity(boxNode, newModel);
		boxNode->AddSceneEntity(newEntity);
		const CollisionHull* const hull = obj->GetCollisionHulls()[0];
		const XMVECTOR scale = XMVectorSubtract(hull->maxBounds, hull->minBounds);
		const XMVECTOR rot = XMVectorSet(0, 0, 0, 0);
		boxNode->SetTransformationVector(scale, rot, XMVectorSet(0, 0, 0, 1));
		newNode->AddChildNode(boxNode);*/

		//return newNode;
		{
			//const BadGuy::SpecialBadGuyData* badGuyData = static_cast<const BadGuy::SpecialBadGuyData*>(objectData.specialData);

#ifdef _NO_METRO
			/*for(int a = 0; a < objectData.object->GetThrusters().size(); ++a)
			{
				{
					SceneNode* particleNode = new SceneNode(this->sceneGraph);
					newNode->AddChildNode(particleNode, std::to_string(a));			
					ParticleSystem* testParticleSystem = new ParticleSystem(XMVectorSet(0, 0, 0, 1),
						FractalTreeResources::ParticleSystemDefinitions[FractalTreeResources::BasicFireTrail]);
					testParticleSystem->particleSystemDefinition.particleEmissionDirection[0] = -((objectData.object->GetThrusters()))[a]->direction;
					testParticleSystem->particleSystemDefinition.particleRadius *= (1 - .1091f);//+ (*(badGuyData->thrusters))[a]->currentMagnitude);
					//testParticleSystem->offset = ((objectData.object->GetThrusters()))[a]->offset;
					testParticleSystem->enabled = false;
					ParticleEntity* testParticleEntity = new ParticleEntity(particleNode, this->renderer->CreateStandardParticleSystemModel(testParticleSystem->particleSystemDefinition.maxConcurrentParticles), testParticleSystem);
					particleNode->SetTransformationVector(XMVectorSet(1,1,1,1), XMVectorSet(0,0,0,1), ((objectData.object->GetThrusters()))[a]->offset);
					particleNode->AddParticleSceneEntity(testParticleEntity);
				}
				
			}*/
#endif
		}
		return newNode;
	}

	void ActorRendererBindingGraph::OnDeleteSceneNodeFromObjectData(SceneNode* sceneNode, const ActorData& previousObjectData, LocalObjectData* localBindingData)
	{
		switch (previousObjectData.objectType)
		{
		case ObjectTypes::Projectile:
			if(sceneNode == this->litNode2)
			{
				this->litNode2 = NULL;
				this->litNode = NULL;
			}
			break;
		case ObjectTypes::IneffectiveMissile:
			{
				if(sceneNode == this->litNode2)
				{
					this->litNode2 = NULL;
					this->litNode = NULL;
				}

				SceneNode* particleNode = new SceneNode(this->sceneGraph, true, 1100);
				particleNode->SetTranslation(sceneNode->GetTranslation());
				
				ParticleSystem* testParticleSystem = new ParticleSystem(XMVectorSet(0, 0, 0, 1), 
					//FractalTreeResources::ParticleSystemDefinitions[FractalTreeResources::GiantExplosion2]);
					FractalTreeResources::ParticleSystemDefinitions[FractalTreeResources::HitExplosion]);
				testParticleSystem->useQuads = true;
				ParticleEntity* testParticleEntity = new ParticleEntity(particleNode, this->renderer->CreateStandardParticleSystemModel(testParticleSystem->particleSystemDefinition.maxConcurrentParticles), testParticleSystem);
				particleNode->AddParticleSceneEntity(testParticleEntity);
			}
			break;
		case ObjectTypes::DecentMissile:
			{
				if(sceneNode == this->litNode2)
				{
					this->litNode2 = NULL;
					this->litNode = NULL;
				}
				
				SceneNode* particleNode = new SceneNode(this->sceneGraph, true, 300);
				particleNode->SetTranslation(sceneNode->GetTranslation());
				
				ParticleSystem* testParticleSystem = new ParticleSystem(XMVectorSet(0, 0, 0, 1), 
					FractalTreeResources::ParticleSystemDefinitions[FractalTreeResources::HitExplosion]);
				testParticleSystem->useQuads = true;
				ParticleEntity* testParticleEntity = new ParticleEntity(particleNode, this->renderer->CreateStandardParticleSystemModel(testParticleSystem->particleSystemDefinition.maxConcurrentParticles), testParticleSystem);
				particleNode->AddParticleSceneEntity(testParticleEntity);
			}
			break;
		default:
			break;
		}
	}

	void ActorRendererBindingGraph::updateTreeActorSceneNode(SceneNode* sceneNode, const ActorData& objectData, LocalObjectData* localBindingData) const
	{
		const SceneEntity* const entity = sceneNode->GetSceneEntity(0);
		if (entity != nullptr)
		{
			const TreeActor* const tree = static_cast<const TreeActor* const>(objectData.object);
			const unsigned int updateVersion = tree->GetUpdateVersion();
			if (updateVersion > localBindingData->previousMeshVersion)
			{
				if (localBindingData->mesh != nullptr)
				{
					delete localBindingData->mesh;
				}


				if (!tree->GetTreeSystem()->TryLock())
				{
					return;
				}

				const std::vector<LSystemSegment*> segments = tree->GetTreeSystem()->getSegmentList();
				const unsigned int rootSegmentCount = tree->GetTreeSystem()->GetRootNodeCount();
				tree->GetTreeSystem()->Unlock();

				if (!entity->model->isLineMesh)
				{
					localBindingData->mesh = new LSystemMesh(segments, 8, rootSegmentCount, false, SystemParameters::UseFaceNormals);
				}
				else
				{
					localBindingData->mesh = new LSystemMesh(segments, 8, rootSegmentCount, true, false);
				}

				this->renderer->UpdateBufferDataFromMesh(entity->model->bufferData, localBindingData->mesh);
				localBindingData->previousMeshVersion = updateVersion;

				delete localBindingData->mesh;
				localBindingData->mesh = nullptr;
			}
		}
	}

	void ActorRendererBindingGraph::terrainWorkerThread(ActorRendererBindingGraph* bindingGraph)
	{
		while (true)
		{
			bindingGraph->terrainCellMutex.lock();

			if (!bindingGraph->funcQueue.empty())
			{
				auto func = bindingGraph->funcQueue.top();
				bindingGraph->funcQueue.pop();
				bindingGraph->terrainCellMutex.unlock();
				func.generationFunc();

			}
			else
			{
				bindingGraph->terrainCellMutex.unlock();
				Sleep(6);
			}
		}
	}

	int generationCount = 0;
	double totalGenerationTime = 0;

	void ActorRendererBindingGraph::addTerrainCellNode(TerrainCellMesh* const cell)
	{
		if (this->terrainCellNodeCount >= this->terrainCellNodesList.size())
		{
			this->terrainCellNodesList.push_back(cell);
		}
		else
		{
			this->terrainCellNodesList[this->terrainCellNodeCount] = cell;
		}

		++this->terrainCellNodeCount;
	}

	void ActorRendererBindingGraph::removeTerrainCellNode(const unsigned int index, const bool beingReplaced)
	{
		const TerrainCellMesh* const cell = this->terrainCellNodesList[index];
		if (index < this->terrainCellNodeCount - 1)
		{
			this->terrainCellNodesList[index] = this->terrainCellNodesList[this->terrainCellNodeCount - 1];
			this->terrainCellNodesList[index]->index = index;
		}

		--this->terrainCellNodeCount;

		//if (!beingReplaced)
		{
			const long long key = this->getTerrainCellKey(cell->cellX, cell->cellZ);
			auto existingMesh = this->terrainCellNodes.find(key);
			if (existingMesh != this->terrainCellNodes.end())
			{
				this->terrainCellNodes[key] = nullptr;
			}
		}

		cell->sceneEntity->parentNode->RemoveModelEntity(cell->sceneEntity);
		delete cell->mesh;
		delete cell;
	}

	TerrainCellMesh* ActorRendererBindingGraph::queueTerrainCell(
		const int cellX,
		const int cellZ,
		const float cellWidth,
		const XMFLOAT4& groundLoc,
		const ProceduralGeneration::PerlinHeightMapGenerator* const generator,
		const int cellToReplace)
	{
		TerrainCellMesh* const cellMesh = new TerrainCellMesh();
		cellMesh->cellX = cellX;
		cellMesh->cellZ = cellZ;
		cellMesh->center = XMVectorSet(static_cast<float>(cellX * cellWidth), 0, static_cast<float>(cellZ * cellWidth), 0);
		

		XMFLOAT4 boundindGroundCenter;
		XMStoreFloat4(&boundindGroundCenter, cellMesh->center);
		boundindGroundCenter.y = 0;

		const XMVECTOR dist = XMVector3Length(XMVectorSubtract(XMLoadFloat4(&groundLoc), XMLoadFloat4(&boundindGroundCenter)));
		XMFLOAT4 distFloat;
		XMStoreFloat4(&distFloat, dist);
		float gridWidth = powf(2.f, static_cast<float>(FractalTreeResources::gridCellWidth));
		float width = static_cast<int>(cellWidth / gridWidth) + 1;
		bool useLod = true;

		if (FractalTreeResources::UseLOD)
		{
			int whichLOD = static_cast<int>(distFloat.x / FractalTreeResources::lodDistanceFactor);
			//only build lower resolution version of mesh because it is very far away.
			if (whichLOD > LOD_COUNT + 2 && FractalTreeResources::unloadFarCells)
			{
				whichLOD = LOD_COUNT - 1;//objectData.object->GetMesh()->lodLevel - 1;
				useLod = false;
				gridWidth = powf(2, whichLOD) * gridWidth;
				width = (width - 1) / powf(2, whichLOD) + 1;
				cellMesh->isLodMesh = true;
			}
			else
			{
				cellMesh->isLodMesh = false;
			}
		}

		const ModelShaderData shaderData(this->renderer->GetVertexShader(FractalTreeRenderer::TerrainVertexShader),
			this->renderer->GetPixelShader(FractalTreeRenderer::SimpleMountainPixelShader),
			this->renderer->GetInputLayout(FractalTreeRenderer::TerrainInputLayout),
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			1,
			1,
			this->renderer->GetShaderResourceView(FractalTreeRenderer::MountainResourceView),
			this->renderer->GetSamplerState(FractalTreeRenderer::TerrainSamplerState),
			NULL,
			NULL,
			NULL);

		cellMesh->shaderData = shaderData;
		auto generationLambda = [shaderData, this, generator, boundindGroundCenter, cellMesh, gridWidth, width, useLod, cellToReplace]()
		{
			QueryPerformanceCounter(&cellMesh->startTime);
			UniformHeightMapMesh* const newMesh = new UniformHeightMapMesh(width, width, gridWidth, boundindGroundCenter.x,
				boundindGroundCenter.z, useLod, FractalTreeResources::terrainTextureRepeatCount, FractalTreeResources::alternateCellOrientation);
			newMesh->SetGenerator(generator, true);
			cellMesh->mesh = newMesh;
			this->terrainCellQueueMutex.lock();
			this->completedTerrainCells.push(cellMesh);
			this->terrainCellQueueMutex.unlock();
			if (cellToReplace >= 0)
			{
				//removeTerrainCellNode(cellToReplace, true);
			}
		};

		if (FractalTreeResources::useTerrainThread)
		{
			this->terrainCellMutex.lock();
			funcQueue.push(TerrainCellGeneratorObject{distFloat.x, generationLambda });
			this->terrainCellMutex.unlock();
		}
		else
		{
			generationLambda();
		}

		return cellMesh;
	}

	void ActorRendererBindingGraph::updateTerrainSceneNode(SceneNode * sceneNode, const ActorData & objectData, LocalObjectData * localBindingData)
	{
		const Terrain* const terrainCell = static_cast<const Terrain*>(objectData.object);


		XMFLOAT4 groundLoc;
		XMStoreFloat4(&groundLoc, this->sceneGraph->GetCameraLocation());
		const float cellWidth = terrainCell->getCellWidth();
		const int cellX = static_cast<int>(groundLoc.x / cellWidth);
		const int cellZ = static_cast<int>(groundLoc.z / cellWidth);

		const long long key = this->getTerrainCellKey(cellX, cellZ);
		auto existingMesh = this->terrainCellNodes.find(key);

		if (existingMesh == this->terrainCellNodes.end() || existingMesh->second == nullptr)
		{
			const int count = FractalTreeResources::cellGenerationCount;
			for (int a = cellX - count; a < cellX + count; ++a) 
			{
				for (int b = cellZ - count; b < cellZ + count; ++b) 
				{
					const long long newKey = ((long long)a) * 2147483647L + (long long)b;
					auto potentialNewMeshPlace = this->terrainCellNodes.find(newKey);
					if (potentialNewMeshPlace != this->terrainCellNodes.end() && potentialNewMeshPlace->second != nullptr)
					{
						continue;
					}

					TerrainCellMesh* const cellMesh = this->queueTerrainCell(a, b, cellWidth, groundLoc, terrainCell->GetGenerator(), -1);
					this->terrainCellNodes[newKey] = cellMesh;
					cellMesh->key = newKey;
				}
			}		
		}

		std::vector<TerrainCellMesh*> newCells;
		this->terrainCellQueueMutex.lock();
		while (!this->completedTerrainCells.empty())
		{
			newCells.push_back(this->completedTerrainCells.front());
			this->completedTerrainCells.pop();
		}
		this->terrainCellQueueMutex.unlock();

		for (int a = 0; a < newCells.size(); ++a)
		{
			_LARGE_INTEGER renderEndTime;
			QueryPerformanceCounter(&renderEndTime);
			const double generationTime = (renderEndTime.QuadPart - newCells[a]->startTime.QuadPart) / performanceFrequency;
			totalGenerationTime += generationTime;
			UIRenderer::SetDebugString(generationTimeInd, "Average cell generation time: " + std::to_string(totalGenerationTime / ++generationCount) + " (Last: " +
				std::to_string(generationTime) + ")");
			Model* const newModel = Direct3DRenderer::CreateStaticModelFromMesh<TexturedVertex>(newCells[a]->mesh, newCells[a]->shaderData);
			newModel->castsShadow = true;
			newModel->checkBounds = true;
			newCells[a]->model = newModel;
			newModel->renderLOD = false;
			newModel->activeBufferIndex = 0;
			SceneEntity* const newEntity = new SceneEntity(sceneNode, newModel);
			newCells[a]->sceneEntity = newEntity;
			newEntity->useBoundingBox = true;
			sceneNode->AddSceneEntity(newEntity);
			sceneNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);
			newEntity->boundingBox = newCells[a]->mesh->boundingBox;
			newCells[a]->mesh->ClearData();

			this->addTerrainCellNode(newCells[a]);
		}

		groundLoc.y = 0;
		bool dontIncrement = false;
		for (unsigned int a = 0; a < this->terrainCellNodeCount; ++a)
		{
			if (dontIncrement)
			{
				--a;
				dontIncrement = false;
			}
			if (this->terrainCellNodesList[a] == nullptr)
			{
				continue;
			}

			XMFLOAT4 boundindGroundCenter;
			XMStoreFloat4(&boundindGroundCenter, this->terrainCellNodesList[a]->center); boundindGroundCenter.y = 0;

			const XMVECTOR dist = XMVector3Length(XMVectorSubtract(XMLoadFloat4(&groundLoc), XMLoadFloat4(&boundindGroundCenter)));
			XMFLOAT4 distFloat;
			XMStoreFloat4(&distFloat, dist);

			if (FractalTreeResources::UseLOD)
			{
				int whichLOD = static_cast<int>(distFloat.x / FractalTreeResources::lodDistanceFactor);
				bool meshDeleted = false;
				if (whichLOD >= LOD_COUNT)
				{
					if (whichLOD > LOD_COUNT + 2 && FractalTreeResources::unloadFarCells)
					{
						//unload existing full resolution mesh to be recreated using only low resolution version
						//because it is far away
						if (!this->terrainCellNodesList[a]->isLodMesh)
						{
							meshDeleted = true;
							const int oldX = this->terrainCellNodesList[a]->cellX;
							const int oldZ = this->terrainCellNodesList[a]->cellZ;
							this->removeTerrainCellNode(a, false);
							dontIncrement = true;
							this->queueTerrainCell(oldX, oldZ, cellWidth, groundLoc, terrainCell->GetGenerator(), a);
						}
						else if (whichLOD > LOD_COUNT + 6 && FractalTreeResources::unloadFarCells)
						{
							//so far away; remove and don't rebuild.
							meshDeleted = true;
							this->removeTerrainCellNode(a, false);
							dontIncrement = true;
						}
						else
						{
							whichLOD = this->terrainCellNodesList[a]->mesh->lodLevel - 1;
						}
					}
					else
					{
						if (this->terrainCellNodesList[a]->isLodMesh && FractalTreeResources::unloadFarCells)
						{
							meshDeleted = true;//??
						}
						else
						{
							whichLOD = this->terrainCellNodesList[a]->mesh->lodLevel - 1;
						}
					}
				}
				else if (this->terrainCellNodesList[a]->isLodMesh && FractalTreeResources::unloadFarCells)
				{
					meshDeleted = true;
					const int oldX = this->terrainCellNodesList[a]->cellX;
					const int oldZ = this->terrainCellNodesList[a]->cellZ;
					this->removeTerrainCellNode(a, true);
					dontIncrement = true;
					this->queueTerrainCell(oldX, oldZ, cellWidth, groundLoc, terrainCell->GetGenerator(), a);
				}
				
				if (!meshDeleted)
				{
					this->terrainCellNodesList[a]->model->activeBufferIndex = whichLOD;
				}
			}
			else
			{
				this->terrainCellNodesList[a]->model->activeBufferIndex = 0;
			}
			
		}

		if (FractalTreeResources::displayResourceVoxels)
		{
			if (this->voxelGridNode == nullptr)
			{
				this->voxelGridNode = new SceneNode(this->sceneGraph);
			}

			if (!this->voxelGridNode->IsVisible())
			{
				this->voxelGridNode->SetVisible(true);
			}

			const vector<pair<unsigned long long, XMVECTOR> > voxels = terrainCell->GetGrowthSimulation()->GetTerrainResources()->GetKeyList();

			for (const pair<unsigned long long, XMVECTOR> v : voxels)
			{
				if (this->seenVoxelKeys.find(v.first) == this->seenVoxelKeys.end())
				{
					SceneNode* const newNode = new SceneNode(this->sceneGraph);
					Model* const newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
					newModel->deleteBuffer = false;
					SceneEntity* newEntity = new SceneEntity(newNode, newModel);
					newNode->AddSceneEntity(newEntity);
					const float sz = VOXEL_SIZE;
					const XMVECTOR translation = XMVectorMultiply(XMVectorSet(sz, sz, sz, 1), v.second);
					newNode->SetTransformationVector(XMVectorSet(sz, sz, sz, 1), XMVectorSet(0, 0, 0, 1), translation);
					this->seenVoxelKeys.emplace(v.first);
					this->voxelGridNode->AddChildNode(newNode);
				}
			}
		}
		else if (this->voxelGridNode != nullptr)
		{
			this->voxelGridNode->Remove();
			this->seenVoxelKeys.clear();
			this->voxelGridNode = nullptr;
		}
	}

	SceneNode* ActorRendererBindingGraph::createTestChainLinkSceneNode(const ActorData& objectData, LocalObjectData* const localBindingData)
	{
		const TestChainLink* const chainLink = static_cast<const TestChainLink*>(objectData.object);
		SceneNode* const newNode = new SceneNode(this->sceneGraph);
		Model* const newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		newModel->deleteBuffer = false;
		SceneEntity* newEntity = new SceneEntity(newNode, newModel);
		newNode->SetLocalScale(XMVectorSet(3, chainLink->length, 3, 0));
		newNode->AddSceneEntity(newEntity);

		return newNode;
	}

	SceneNode* ActorRendererBindingGraph::createTestSkeletonArmSegmentSceneNode(const ActorData& objectData, LocalObjectData* const localBindingData)
	{
		const TestSkeletonArmSegment* const chainLink = static_cast<const TestSkeletonArmSegment*>(objectData.object);
		SceneNode* const newNode = new SceneNode(this->sceneGraph);
		Model* const newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		newModel->deleteBuffer = false;
		SceneEntity* newEntity = new SceneEntity(newNode, newModel);
		newNode->SetLocalScale(XMVectorSet(3, chainLink->length, 3, 0));
		newNode->AddSceneEntity(newEntity);

		return newNode;
	}


	SceneNode* ActorRendererBindingGraph::createTestSkeletonUserSceneNode(const ActorData& objectData, LocalObjectData* const localBindingData)
	{
		const TestUserOfSkeleton* const skeletonObject = static_cast<const TestUserOfSkeleton*>(objectData.object);
		const Skeleton* const skeleton = skeletonObject->GetSkeleton();
		SceneNode* const newNode = new SceneNode(this->sceneGraph);

		localBindingData->skeletonMeshData = new SkeletonMeshData();
		localBindingData->skeletonMeshData->segmentNodes.resize(skeleton->GetSegments().size());
		if (FractalTreeResources::showIKSkeleton)
		{
			const vector<SkeletonSegment*>& segments = skeleton->GetSegments();
			for (int a = 0; a < segments.size(); ++a)
			{
				const SkeletonSegment* const segment = segments[a];
				SceneNode* const segmentNode = new SceneNode(this->sceneGraph);
				newNode->AddChildNode(segmentNode);
				Model* const newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
				newModel->deleteBuffer = false;
				SceneEntity* const newEntity = new SceneEntity(segmentNode, newModel);
				XMMATRIX transform = segment->GetFullTransformationNoCache();
				segmentNode->SetManualTransform(transform);
				segmentNode->SetUseManualTransform(true);
				segmentNode->AddSceneEntity(newEntity);
				segmentNode->SetLocalScale(segment->GetScale());
				segmentNode->SetTint(XMVectorSet(.7f, .4f, .51f, 1));
				localBindingData->skeletonMeshData->segmentNodes[segment->GetLocalId()] = segmentNode;
			}
		}

		SceneNode* const newNode2 = new SceneNode(this->sceneGraph);
		Model* const newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["teapot.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		newModel->deleteBuffer = false;
		SceneEntity* const newEntity = new SceneEntity(newNode2, newModel);
		newNode2->SetTranslation(skeletonObject->destination);
		newNode2->SetScale(XMVectorSet(5.5f, 5.5f, 5.5f, 0));
		newNode2->SetTint(XMVectorSet(1, 0, 0, 1));
		newNode2->AddSceneEntity(newEntity);
		newNode->AddChildNode(newNode2, "dest");

		SceneNode* const newNode3 = new SceneNode(this->sceneGraph);
		Model* const newModel2 = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		newModel2->deleteBuffer = false;
		SceneEntity* const newEntity2 = new SceneEntity(newNode3, newModel2);
		//newNode3->SetTranslation(skeletonObject->destination);
		newNode3->SetScale(XMVectorSet(15.5f, 15.5f, 15.5f, 0));
		newNode3->SetTint(XMVectorSet(1, 0, 0, 1));
		newNode3->AddSceneEntity(newEntity2);
		newNode->AddChildNode(newNode3, "start");


		if (FractalTreeResources::ikShowEnd)
		{
			SceneNode* const newNode4 = new SceneNode(this->sceneGraph);
			Model* const newModel3 = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["teapot.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
			newModel3->deleteBuffer = false;
			SceneEntity* const newEntity3 = new SceneEntity(newNode4, newModel3);
			newNode4->SetTranslation(skeletonObject->destination);
			newNode4->SetScale(XMVectorSet(5.5f, 5.5f, 5.5f, 0));
			newNode4->AddSceneEntity(newEntity3);
			newNode->AddChildNode(newNode4, "end");
		}
		return newNode;
	}

	void ActorRendererBindingGraph::updateTestSkeletonUserSceneNode(SceneNode* const sceneNode, const ActorData& objectData, LocalObjectData* const localBindingData) const
	{
		const TestUserOfSkeleton* const skeletonObject = static_cast<const TestUserOfSkeleton*>(objectData.object);
		const Skeleton* const skeleton = skeletonObject->GetSkeleton();
		const vector<SkeletonSegment*>& segments = skeleton->GetSegments();

		if (FractalTreeResources::showIKSkeleton && (skeleton->GetFailFrames() == 0 || !FractalTreeResources::onlyIkSolution))
		{
			for (int a = 0; a < segments.size(); ++a)
			{
				const SkeletonSegment* const segment = segments[a];
				SceneNode* const segmentNode = localBindingData->skeletonMeshData->segmentNodes[a];
				segmentNode->SetManualTransform(segment->GetFullTransformationNoCache());
				segmentNode->SetLocalTranslation(XMVectorSet(0, -segment->GetLength() * .5f, 0, 1));
			}
		}

		sceneNode->GetChildByName("dest")->SetTranslation(skeletonObject->destination);
	}

	void ActorRendererBindingGraph::updateSkeleton(const ActorData& objectData, LocalObjectData* const localBindingData, const XMVECTOR& dest,
		const Skeleton* const skeleton, SceneNode* rootNode) {
		if (localBindingData->skeletonMeshData != nullptr) {
			for (SceneNode* const n : localBindingData->skeletonMeshData->segmentNodes) {
				n->Remove();
			}

			delete localBindingData->skeletonMeshData;
		}
		localBindingData->skeletonMeshData = new SkeletonMeshData();
		localBindingData->skeletonMeshData->segmentNodes.resize(skeleton->GetSegments().size());
		if (FractalTreeResources::showIKSkeleton)
		{
			const vector<SkeletonSegment*>& segments = skeleton->GetSegments();
			for (int a = 0; a < segments.size(); ++a)
			{
				const SkeletonSegment* const segment = segments[a];
				SceneNode* const segmentNode = new SceneNode(this->sceneGraph);
				rootNode->AddChildNode(segmentNode);
				Model* const newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
				newModel->deleteBuffer = false;
				SceneEntity* const newEntity = new SceneEntity(segmentNode, newModel);
				const XMMATRIX transform = segment->GetFullTransformationNoCache();
				segmentNode->SetManualTransform(transform);
				segmentNode->SetUseManualTransform(true);
				segmentNode->AddSceneEntity(newEntity);
				segmentNode->SetLocalScale(segment->GetScale());
				segmentNode->SetTint(XMVectorSet(.7f, .4f, .51f, 1));
				localBindingData->skeletonMeshData->segmentNodes[segment->GetLocalId()] = segmentNode;
			}
		}

		SceneNode* const newNode2 = new SceneNode(this->sceneGraph);
		Model* const newModel = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["teapot.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		newModel->deleteBuffer = false;
		SceneEntity* const newEntity = new SceneEntity(newNode2, newModel);
		newNode2->SetTranslation(dest);
		newNode2->SetScale(XMVectorSet(5.5f, 5.5f, 5.5f, 0));
		newNode2->SetTint(XMVectorSet(1, 0, 0, 1));
		newNode2->AddSceneEntity(newEntity);
		rootNode->AddChildNode(newNode2, "dest");

		SceneNode* const newNode3 = new SceneNode(this->sceneGraph);
		Model* const newModel2 = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
		newModel2->deleteBuffer = false;
		SceneEntity* const newEntity2 = new SceneEntity(newNode3, newModel2);
		//newNode3->SetTranslation(skeletonObject->destination);
		newNode3->SetScale(XMVectorSet(15.5f, 15.5f, 15.5f, 0));
		newNode3->SetTint(XMVectorSet(1, 0, 0, 1));
		newNode3->AddSceneEntity(newEntity2);
		rootNode->AddChildNode(newNode3, "start");


		if (FractalTreeResources::ikShowEnd)
		{
			SceneNode* const newNode4 = new SceneNode(this->sceneGraph);
			Model* const newModel3 = new Model(FractalTreeResources::StaticModels[FractalTreeResources::ResourceMap["teapot.m"]], renderer->CreateStandardModelShaderData(FractalTreeRenderer::SimpleVertexShader, FractalTreeRenderer::SimplePixelShader, FractalTreeRenderer::DefaultInputLayout));
			newModel3->deleteBuffer = false;
			SceneEntity* const newEntity3 = new SceneEntity(newNode4, newModel3);
			newNode4->SetTranslation(dest);
			newNode4->SetScale(XMVectorSet(5.5f, 5.5f, 5.5f, 0));
			newNode4->AddSceneEntity(newEntity3);
			rootNode->AddChildNode(newNode4, "end");
		}
	}
	SceneNode* ActorRendererBindingGraph::createDynamicSkeletonUserSceneNode(const ActorData& objectData, LocalObjectData* const localBindingData)
	{
		const DynamicUserOfSkeleton* const skeletonObject = static_cast<const DynamicUserOfSkeleton*>(objectData.object);
		const Skeleton* const skeleton = skeletonObject->GetSkeleton();
		SceneNode* const newNode = new SceneNode(this->sceneGraph);

		this->updateSkeleton(objectData, localBindingData, skeletonObject->destination, skeleton, newNode);
		return newNode;
	}

	void ActorRendererBindingGraph::updateDynamicSkeletonUserSceneNode(SceneNode* const sceneNode, const ActorData& objectData, LocalObjectData* const localBindingData)
	{
		const DynamicUserOfSkeleton* const skeletonObject = static_cast<const DynamicUserOfSkeleton*>(objectData.object);
		const Skeleton* const skeleton = skeletonObject->GetSkeleton();
		bool t = skeleton->hasChanged.load();
		const bool f = false;
		bool doAnything = false;
		if (skeleton->hasChanged.compare_exchange_strong(t, false) && t) {
			//for (int a = 0; a < localBindingData->)
			this->updateSkeleton(objectData, localBindingData, skeletonObject->destination, skeleton, sceneNode);
			doAnything = true;
		}
		const vector<SkeletonSegment*>& segments = skeleton->GetSegments();

		if (FractalTreeResources::showIKSkeleton && (doAnything || skeleton->GetFailFrames() == 0 || !FractalTreeResources::onlyIkSolution))
		{
			for (int a = 0; a < segments.size() && a < localBindingData->skeletonMeshData->segmentNodes.size(); ++a)
			{
				const SkeletonSegment* const segment = segments[a];
				SceneNode* const segmentNode = localBindingData->skeletonMeshData->segmentNodes[a];
				segmentNode->SetManualTransform(segment->GetFullTransformationNoCache());
				segmentNode->SetLocalTranslation(XMVectorSet(0, -segment->GetLength() * .5f, 0, 1));
			}
		}

		sceneNode->GetChildByName("dest")->SetTranslation(skeletonObject->destination);
	}

	float anng = 0;
	unsigned int rpin = 0;
	void ActorRendererBindingGraph::UpdateRendererProperties()
	{
#ifndef _NO_GRAPHICS
		if (this->worldSimulation->generateTerrain)
		{
			UIRenderer::SetDebugString(queueDebugIndex, "Cells Queued: " + std::to_string(this->funcQueue.size()));
			UIRenderer::SetDebugString(cellCountIndex, "Cell Count: " + std::to_string(this->terrainCellNodeCount));
		}
		UIRenderer::SetDebugString(objectCountIndex, "Object count: " + std::to_string(this->simulationObjectCount));
#endif
		ang2+=.005f;
		for(unsigned int a = 0; a < this->sceneGraph->GetNodeCount(); ++a)
		{
			if(this->sceneGraph->GetNode(a)->GetParticleEntityCount() > 0)
			{
				const ParticleEntity* const particleEntity = this->sceneGraph->GetNode(a)->GetParticleEntity(0);
				if(particleEntity != NULL)
				{
					this->renderer->UpdateBufferFromParticleSystem(particleEntity->model->bufferData, particleEntity->particleSystem);
				}
			}
		}
		if(!FractalTreeResources::flyMove)
		{
			this->sceneGraph->SetCameraLocation(XMVectorAdd(this->worldSimulation->GetCameraLocation(this->clientID), XMVectorSet(0, camHeight, 0, 0))); //25
			const XMVECTOR facing = this->worldSimulation->GetCameraFacing(this->clientID);
			this->sceneGraph->SetCameraFacing(facing);
			//this->sceneGraph->SetCameraFacing
			//const XMVECTOR sideVec = XMVector3Normalize(XMVector3Cross(this->sceneGraph->GetCameraFacing(), XMVectorSet(0, 1, 0, 0)));
			//this->sceneGraph->SetCameraUp(XMVector3Normalize(XMVector3Cross(sideVec, this->sceneGraph->GetCameraFacing())));
			//this->sceneGraph->SetCameraUp(XMVector3Transform(XMVectorSet(0, 1, 0, 0), XMMatrixRotationRollPitchYawFromVector(this->worldSimulation->GetCameraPitchRollYa(0))));
			
			this->sceneGraph->SetCameraUp(this->worldSimulation->GetCameraUp(this->clientID));
			this->sceneGraph->SetCameraOrientation(this->worldSimulation->GetCameraOrientation(this->clientID));
		}
		else
		{
			XMFLOAT4 newLoc;
			XMStoreFloat4(&newLoc, this->worldSimulation->GetCameraLocation(0));
			//newLoc.y = 2500;
			newLoc.z-=125;
			newLoc.y+= 50;
			this->sceneGraph->SetCameraLocation(XMLoadFloat4(&newLoc));
			this->sceneGraph->SetCameraFacing(-XMVector3Normalize(XMVectorSubtract(XMLoadFloat4(&newLoc), this->worldSimulation->GetCameraLocation(0))));
		}

		if(setPos)
		{
			anng+=.001f;
			const XMMATRIX rotMat = XMMatrixRotationY(anng);
			XMVECTOR lightPos = XMVector3Transform(XMVectorSet(400, 0, 0, 0), rotMat);
			lightPos = XMVectorAdd(XMVectorSet(800, 100, 800, 0), lightPos);
			this->sceneGraph->SetLightPosition(lightPos);
			//this->sceneGraph->SetLightColor(lightPos);
		}

		const float txV = ((++rpin) % 100000) * .00001;

		static float tttt = 0;
		tttt += .01f;
		this->sceneGraph->SetCustomValue(XMVectorSet(txV, txV, tttt, 0));
		setPos = true;
		if (this->skyNode != nullptr)
		{
			this->skyNode->SetTransformationVector(XMVectorSet(20, 20, 20, 0), XMVectorSet(0, 0, 0, 0), this->sceneGraph->GetCameraLocation());
		}
	}

	ActorRendererBindingGraph::~ActorRendererBindingGraph()
	{

	}
}