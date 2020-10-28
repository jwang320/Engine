#include "pch.h"
#include "GameBindingGraph.h"
#include "BlockUniverse.h"
#include "PlatformerCharacter.h"
#include "PlatformerPersonCharacter.h"
#include "GameResources.h"

#include "..\Rendering Engine Standard\SceneNode.h"
#include "..\Rendering Engine Standard\SceneGraph.h"
#include "..\src\ResourceManager.h"
#include "..\Rendering Engine Standard\SceneEntity.h"
#include "..\src\UIRenderer.h"
using namespace RenderingEngine;
using namespace DirectX;

namespace Win8GameRenderingEngine
{
	static int cameraLocationIndex = -1;
	static const XMVECTOR blockColors[] = {
		XMVectorSet(1, 1, 1, 1),
		XMVectorSet(0, 0, 0, 1),
		XMVectorSet(0, 0, 0, 1),
		XMVectorSet(0, 0, 0, 1),
		XMVectorSet(0, 0, 0, 1)
	};

	static const XMVECTOR blockEmissiveColors[] = {
		XMVectorSet(0, 0, 0, 0),
		XMVectorSet(1, 1, 1, 1),
		XMVectorSet(0, 1, 0, 1),
		XMVectorSet(0, 0, 1, 1),
		XMVectorSet(1, 0, 0, 1)
	};

	GameBindingGraph::GameBindingGraph(const GameSimulation* physicalWorldSimulation, RenderingEngine::SceneGraph* sceneGraph, const GameRenderer* renderer) :
		ObjectRendererBindingGraph(physicalWorldSimulation, sceneGraph, renderer),
		clientID(0),
		mouseLoc(0, 0, 0, 0),
		addBlockSceneNode(NULL),
		playerCharacterID(-1),
		light1Node(NULL),
		frameCounter(0)
	{
		this->sceneGraph->SetCameraLocation(XMVectorSet(0, 250, 400, 0));
		this->sceneGraph->SetCameraFacing(XMVector3Normalize(XMVectorSet(0, 0, -1, 0)));
		this->sceneGraph->SetLightPosition(XMVectorSet(50, 10, 50, 0));
		this->sceneGraph->SetSunColor(XMVectorMultiply(XMVectorSet(1, 1, 1, 1), XMVectorReplicate(.5f)));
		this->sceneGraph->SetLightColor(XMVectorSet(1, 1, 1, 1));
		this->sceneGraph->SetLight1Position(XMVectorSet(50, 10, 50, 0));
		this->sceneGraph->SetLight1Color(XMVectorSet(1, 0, 0, 1));
		this->sceneGraph->SetCameraUp(XMVectorSet(0, 1, 0, 0));
		this->sceneGraph->SetSunPosition(XMVector3Normalize(XMVectorSet(0, 2, 1, 0)));
		
		if (cameraLocationIndex != -1)
		{
			RenderingEngine::UIRenderer::HideDebugString(cameraLocationIndex);
		}

		cameraLocationIndex = RenderingEngine::UIRenderer::AddDebugString("Camera Location: ");

		const XMVECTOR newLoc = XMVectorSet(0, 0, 0, 0);
		this->addBlockSceneNode = new SceneNode(this->sceneGraph);
		Model* newModel = new Model(ResourceManager::StaticModels[ResourceManager::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(GameRenderer::SimpleVertexShader, GameRenderer::SimplePixelShader, GameRenderer::DefaultInputLayout));
		newModel->deleteBuffer = false;
		SceneEntity* newEntity = new SceneEntity(addBlockSceneNode, newModel);
		addBlockSceneNode->AddSceneEntity(newEntity);
		addBlockSceneNode->SetTransformationVector(XMVectorSet(GameResources::SquareWidth, GameResources::SquareWidth, GameResources::SquareWidth, 1), XMVectorSet(0, 0, 0, 0), newLoc);
	}

	//CreateSceneNodeFromObjectData controls how the graphics for a particular object in the world are
	//set up when it is created.
	//When defining a new class for an object in the game world, a case should be added to this function for 
	//its object type.
	SceneNode* GameBindingGraph::CreateSceneNodeFromObjectData(const ActorData& objectData, LocalObjectData* localBindingData)
	{
		switch (objectData.objectType)
		{
			case ObjectTypes::BlockUniverse:
				return this->createBlockUniverseSceneNode(objectData, localBindingData);

			case ObjectTypes::PlatformerCharacter:
				return this->createPlatformerCharacterSceneNode(objectData, localBindingData);

			case ObjectTypes::PlatformerPersonCharacter:
				return this->createPlatformerPersonCharacterSceneNode(objectData, localBindingData);
			case ObjectTypes::PlatformerMissile:
				return this->createPlatformerMissileSceneNode(objectData, localBindingData);
			default:
				return this->createPlatformerCharacterSceneNode(objectData, localBindingData); 			
		}
	}

	void GameBindingGraph::UpdateSceneNodeFromObjectData(SceneNode* sceneNode, const ActorData& objectData, LocalObjectData* localBindingData)
	{
		switch (objectData.objectType)
		{
		case ObjectTypes::PlatformerCharacter:
			this->updatePlatformerCharacterSceneNode(sceneNode, objectData, localBindingData);
			break;
		case ObjectTypes::PlatformerPersonCharacter:
			this->updatePlatformerPersonCharacterSceneNode(sceneNode, objectData, localBindingData);
			break;
		case ObjectTypes::BlockUniverse:
			sceneNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);
			sceneNode->SetVelocity(objectData.object->GetVelocity());
			break;
		default:
			this->updatePlatformerCharacterSceneNode(sceneNode, objectData, localBindingData);
			break;
		}	
	}

	void GameBindingGraph::OnDeleteSceneNodeFromObjectData(SceneNode* sceneNode, const ActorData& previousObjectData, LocalObjectData* localBindingData)
	{
		switch (previousObjectData.objectType)
		{
		case ObjectTypes::PlatformerMissile:
			if (sceneNode == this->light1Node)
			{
				this->light1Node = NULL;
			}
			break;
		default:
			break;
		}
	}

	float customValueCounterTest = 0;
	float customValueVel = .01f;
	//This function runs once per frame.
	void GameBindingGraph::UpdateRendererProperties()
	{
		this->sceneGraph->SetCustomValue(XMVectorSet(this->frameCounter, 0, 0, 0));
		++this->frameCounter;
		customValueCounterTest += customValueVel;
		if (customValueCounterTest > 1 || customValueCounterTest < 0)
		{
			customValueVel *= -1;
			//customValueCounterTest = 0;
		}

		if (this->light1Node != NULL)
		{
			XMFLOAT4 nodePos;
			XMStoreFloat4(&nodePos, this->light1Node->GetTranslation());
			this->sceneGraph->SetLight1Position(XMVectorSet(nodePos.x, nodePos.y, 50, 0));
			this->sceneGraph->SetLight1Color(XMVectorSet(1, 0, 0, 1));
		}
		else
		{
			this->sceneGraph->SetLight1Color(XMVectorSet(0, 0, 0, 0));
		}

		XMFLOAT4 tempLoc;
		XMStoreFloat4(&tempLoc, this->sceneGraph->GetCameraLocation());
		std::string locString = std::to_string(static_cast<int>(tempLoc.x)) + ", " + std::to_string(static_cast<int>(tempLoc.y)) + ", " + std::to_string(static_cast<int>(tempLoc.z));
		RenderingEngine::UIRenderer::SetDebugString(cameraLocationIndex, "Camera Location: " + locString);

		if (this->playerCharacterID >= 0 && this->worldSimulation->IsObjectValid(this->playerCharacterID))
		{
			const PlatformerCharacter* playerCharacter = static_cast<PlatformerCharacter*>(this->worldSimulation->GetWorldObjectByGlobalID(this->playerCharacterID));
			if (GameResources::LockCamera)
			{
				XMFLOAT4 playerLoc;
				XMStoreFloat4(&playerLoc, playerCharacter->GetLocation());
				XMFLOAT4 camLoc;
				XMStoreFloat4(&camLoc, this->sceneGraph->GetCameraLocation());

				//this->sceneGraph->SetCameraLocation(XMVectorAdd(this->sceneGraph->GetCameraLocation(), XMVectorSet(0, 150 + playerLoc.y, 0, 0)));
				this->sceneGraph->SetCameraLocation(XMVectorSet(camLoc.x, 150 + playerLoc.y, camLoc.z, 0));
			}

			this->sceneGraph->SetLightPosition(XMVectorAdd(playerCharacter->GetLocation(), XMVectorSet(0, 0, 20, 0)));
			//Display a block graphic under the cursor to denote where a block will be created when
			//the mouse is clicked
			if (this->worldSimulation->IsBlockAddMode())
			{
				this->addBlockSceneNode->SetVisible(true);
				XMFLOAT4 loc;
				XMStoreFloat4(&loc, this->worldSimulation->GetCursorWorldLocation());
				loc.x = static_cast<int>(((loc.x) / GameResources::GridWidth) + (loc.x > 0 ? .5f : -.5f)) * GameResources::GridWidth;
				loc.y = static_cast<int>(((loc.y) / GameResources::GridWidth) + +(loc.y > 0 ? .5f : -.5f)) * GameResources::GridWidth;
				XMVECTOR addLoc = XMLoadFloat4(&loc);
				const float s = 1.4f;
				this->addBlockSceneNode->SetTransformationVector(XMVectorSet(GameResources::SquareWidth * s, GameResources::SquareWidth * s, GameResources::SquareWidth * s, 1), XMVectorSet(0, 0, 0, 0), addLoc);

				//We want the graphic to be green if the location is valid for block placement; red otherwise.
				if (playerCharacter->IsValidBlockAddLocation(addLoc))
				{
					this->addBlockSceneNode->GetSceneEntity(0)->model->diffuseTint = XMVectorSet(.8f, .8f, .8f, .8f);
					this->addBlockSceneNode->GetSceneEntity(0)->model->tint = XMVectorSet(0, 1, 0, 0);
				}
				else if (playerCharacter->IsValidBlockReplaceLocation(addLoc))
				{
					this->addBlockSceneNode->GetSceneEntity(0)->model->diffuseTint = XMVectorSet(.8f, .8f, .8f, .8f);
					this->addBlockSceneNode->GetSceneEntity(0)->model->tint = XMVectorSet(0, 0, 1, 0);
				}
				else
				{
					this->addBlockSceneNode->GetSceneEntity(0)->model->tint = XMVectorSet(1, 0, 0, 0);
					this->addBlockSceneNode->GetSceneEntity(0)->model->diffuseTint = XMVectorSet(.8f, .8f, .8f, .8f);
				}
				
			}
			else
			{
				this->addBlockSceneNode->SetVisible(false);
			}
		}
		else
		{
			this->playerCharacterID = -1;
		}
	}

	SceneNode* GameBindingGraph::createBlockUniverseSceneNode(const ActorData& objectData, LocalObjectData* localBindingData)
	{
		SceneNode* newNode = new SceneNode(this->sceneGraph);

		const BlockUniverse* const blockUniverse = static_cast<const BlockUniverse*>(objectData.object);

		/*{
			SceneNode* backGroundNode = new SceneNode(this->sceneGraph);
			Model* newModel = new Model(ResourceManager::StaticModels[ResourceManager::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(GameRenderer::SimpleVertexShader, GameRenderer::SimplePixelShader, GameRenderer::DefaultInputLayout));
			newModel->deleteBuffer = false;
			newModel->modelShaderData.samplerStateSlot = 1;
			SceneEntity* newEntity = new SceneEntity(backGroundNode, newModel);
			newModel->customValue = XMVectorSet(0, 200, 0, 1);
			backGroundNode->AddSceneEntity(newEntity);
			backGroundNode->SetTransformationVector(XMVectorSet(3000, 3000, 1, 1), XMVectorSet(0, 0, 0, 0), XMVectorSet(1500, 1500, -blockUniverse->GetGridSpacing(), 0));
			newNode->AddChildNode(backGroundNode);
		}*/

		if (GameResources::UseInstancing)
		{
			InstanceBufferData* instanceBuffer = this->renderer->CreateInstanceBufferData(blockUniverse->GetTotalBlockCount() * 2);

			Model* newModel = new Model(ResourceManager::StaticModels[ResourceManager::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(GameRenderer::InstancedFoliageVertexShader, GameRenderer::SimplePixelShader, GameRenderer::InstancedInputLayout));
			newModel->deleteBuffer = false;
			SceneEntity* newEntity = new SceneEntity(newNode, newModel);
			newModel->customValue = XMVectorSet(0, 0, 0, 1);
			newNode->AddSceneEntity(newEntity);
			newNode->SetTransformationVector(XMVectorSet(1, 1, 1, 1), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 0, 0, 0));

			newModel->instanceBufferData = instanceBuffer;

			const XMMATRIX scaleMatrix = XMMatrixScaling(blockUniverse->GetSquareWidth(), blockUniverse->GetSquareWidth(), blockUniverse->GetSquareWidth());
			const XMMATRIX scaleMatrix2 = XMMatrixScaling(blockUniverse->GetGridSpacing(), blockUniverse->GetGridSpacing() - blockUniverse->GetSquareWidth(), blockUniverse->GetGridSpacing());
			ColoredInstanceVertex* instanceData = new ColoredInstanceVertex[blockUniverse->GetTotalBlockCount()*2];

			int blockIndex = 0;
			int indexOffset = 0;
			for (int x = 0; x < blockUniverse->GetWidth(); ++x)
			{
				for (int y = 0; y < blockUniverse->GetHeight(); ++y)
				{
					if (blockUniverse->GetBlockType(x, y) == BlockUniverse::REGULAR)
					{
						const XMMATRIX translateMatrix = XMMatrixTranslationFromVector(XMVectorSet((x) * blockUniverse->GetGridSpacing(), (y) * blockUniverse->GetGridSpacing(), 0, 1));
						bool edges[4];
						blockUniverse->FindBlockEdges(x, y, edges);
						//if (edges[0] || edges[1] || edges[2] || edges[3])
						if (edges[3])
						{
							instanceData[blockIndex + indexOffset].instanceTransform = XMMatrixMultiply(scaleMatrix, translateMatrix);
							//instanceData[blockIndex].emissiveColor = XMVectorSet(.75f, .75f, 1, 0);
							//instanceData[blockIndex].diffuseColor = XMVectorSet(.25f, .25f, 0, 1);
							instanceData[blockIndex + indexOffset].emissiveColor = XMVectorSet(0, 0, 0, 0);
							instanceData[blockIndex + indexOffset].diffuseColor = XMVectorSet(1, 1, 1, 1);
							instanceData[blockIndex + indexOffset].specularExponent = 55;
							++indexOffset;
							const XMMATRIX translateMatrix2 = XMMatrixTranslationFromVector(XMVectorAdd(XMVectorSet((x) * blockUniverse->GetGridSpacing(), (y) * blockUniverse->GetGridSpacing(), 0, 1),
								XMVectorSet(0, blockUniverse->GetGridSpacing() * .5f, 0, 0)));
							instanceData[blockIndex + indexOffset].instanceTransform = XMMatrixMultiply(scaleMatrix2, translateMatrix2);
							//instanceData[blockIndex].emissiveColor = XMVectorSet(.75f, .75f, 1, 0);
							//instanceData[blockIndex].diffuseColor = XMVectorSet(.25f, .25f, 0, 1);
							instanceData[blockIndex + indexOffset].emissiveColor = XMVectorSet(0, 0, 0, 0);
							instanceData[blockIndex + indexOffset].diffuseColor = XMVectorSet(0, 0, 1, .9f);
							instanceData[blockIndex + indexOffset].specularExponent = 55;
						}
						else
						{
							instanceData[blockIndex + indexOffset].instanceTransform = XMMatrixMultiply(scaleMatrix, translateMatrix);
							instanceData[blockIndex + indexOffset].emissiveColor = XMVectorSet(0, 0, 0, 0);
							instanceData[blockIndex + indexOffset].diffuseColor = XMVectorSet(1, 1, 1, 1);
							instanceData[blockIndex + indexOffset].specularExponent = 55;
						}
	//					instanceData[blockIndex].specularColor = XMVectorSet(1, 1, 1, 1);
					
						++blockIndex;
					}
				}
			}

			newModel->instanceBufferData->instanceCount = blockUniverse->GetTotalBlockCount() + indexOffset;
			this->renderer->UpdateInstanceBufferData(newModel->instanceBufferData, instanceData);
			newNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);
		
			delete[] instanceData;
		}
		else
		{
			for (int x = 0; x < blockUniverse->GetWidth(); ++x)
			{
				for (int y = 0; y < blockUniverse->GetHeight(); ++y)
				{
					if (blockUniverse->GetBlockType(x, y) == BlockUniverse::REGULAR)
					{
						const XMVECTOR newLoc = XMVectorSet((x) * blockUniverse->GetGridSpacing(), (y) * blockUniverse->GetGridSpacing(), 0, 0);
						SceneNode* node = new SceneNode(this->sceneGraph);
						Model* newModel = new Model(ResourceManager::StaticModels[ResourceManager::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(GameRenderer::SimpleVertexShader, GameRenderer::SimplePixelShader, GameRenderer::DefaultInputLayout));
						newModel->deleteBuffer = false;
						SceneEntity* newEntity = new SceneEntity(node, newModel);
						node->AddSceneEntity(newEntity);
						node->SetTransformationVector(XMVectorSet(blockUniverse->GetSquareWidth(), blockUniverse->GetSquareWidth(), blockUniverse->GetSquareWidth() * GameResources::WorldZDepth, 1), XMVectorSet(0, 0, 0, 0), newLoc);
						newNode->AddChildNode(node);
					}
				}
			}
		}

		return newNode;
	}

	SceneNode* GameBindingGraph::createPlatformerCharacterSceneNode(const ActorData& objectData, LocalObjectData* localBindingData, bool createCrosshair)
	{
		const PlatformerCharacter* const platformerCharacter = static_cast<const PlatformerCharacter*>(objectData.object);
		const BlockUniverse* const blockUniverse = platformerCharacter->GetParentBlockUniverse();
		SceneNode* newNode = new SceneNode(this->sceneGraph);

		if (createCrosshair)
		{
			const PlatformerPersonCharacter* const platformerPersonCharacter = static_cast<const PlatformerPersonCharacter*>(objectData.object);

			this->playerCharacterID = platformerPersonCharacter->GetGlobalID();

			SceneNode* newNode2 = new SceneNode(this->sceneGraph);
			Model* newModel = new Model(ResourceManager::StaticModels[ResourceManager::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(GameRenderer::SimpleVertexShader, GameRenderer::SimplePixelShader, GameRenderer::DefaultInputLayout));
			newModel->deleteBuffer = false;
			SceneEntity* newEntity = new SceneEntity(newNode2, newModel);
			newNode2->AddSceneEntity(newEntity);
			newNode2->SetTransformationVector(XMVectorSet(5, 5, 5, 1), XMVectorReplicate(0), XMVectorSet(150, 0, 0, 0));
			newNode->AddChildNode(newNode2, "crosshair");
			newModel->diffuseTint = XMVectorSet(.2f, 1, .2f, .35f);
			newModel->tint = XMVectorSet(.2f, 1, .2f, 0);
		}

		for (int a = 0; a < platformerCharacter->GetBlockList().size(); ++a)
		{
			SceneNode* node = new SceneNode(this->sceneGraph);
			Model* newModel = new Model(ResourceManager::StaticModels[ResourceManager::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(GameRenderer::SimpleVertexShader, GameRenderer::SimplePixelShader, GameRenderer::DefaultInputLayout));
			newModel->deleteBuffer = false;
			SceneEntity* newEntity = new SceneEntity(node, newModel);
			BlockCoordinate c = platformerCharacter->GetBlockList()[a];
			const XMVECTOR newLoc = XMVectorSet((c.x * blockUniverse->GetGridSpacing()), (c.y * blockUniverse->GetGridSpacing()), 0, 0);
			const XMVECTOR blockScale = XMVectorSet(blockUniverse->GetSquareWidth(), blockUniverse->GetSquareWidth(), blockUniverse->GetSquareWidth() * GameResources::WorldZDepth, 1);
			node->AddSceneEntity(newEntity);
			node->SetTransformationVector(blockScale, objectData.rollPitchYaw, newLoc);
			newNode->AddChildNode(node);
			newModel->diffuseTint = blockColors[platformerCharacter->GetBlockList()[a].blockType];
			newModel->tint = blockEmissiveColors[platformerCharacter->GetBlockList()[a].blockType];
		}
			
		newNode->SetTransformationVector(XMVectorSet(1, 1, 1, 0), objectData.rollPitchYaw, objectData.location);
		return newNode;
	}

	SceneNode* GameBindingGraph::createPlatformerPersonCharacterSceneNode(const ActorData& objectData, LocalObjectData* localBindingData)
	{
		localBindingData->nodeOffset = 1;
		SceneNode* sceneNode = this->createPlatformerCharacterSceneNode(objectData, localBindingData, true);

		return sceneNode;
	}

	SceneNode* GameBindingGraph::createPlatformerMissileSceneNode(const ActorData& objectData, LocalObjectData* localBindingData)
	{
		SceneNode* newNode = this->createPlatformerCharacterSceneNode(objectData, localBindingData);
		if (this->light1Node == NULL)
		{
			this->light1Node = newNode;
		}

		return newNode;
	}

	void GameBindingGraph::updatePlatformerCharacterSceneNode(SceneNode* sceneNode, const ActorData& objectData, LocalObjectData* localBindingData)
	{
		const PlatformerCharacter* const platformerCharacter = static_cast<const PlatformerCharacter*>(objectData.object);
		const BlockUniverse* const blockUniverse = platformerCharacter->GetParentBlockUniverse();
		const float blockSize = blockUniverse->GetGridSpacing();

		//check if the block configuration has been modified
		if (platformerCharacter->GetModificationVersion() > localBindingData->previousMeshVersion)
		{
			for (int a = 0; a < platformerCharacter->GetBlockList().size() && a < sceneNode->GetChildCount(); ++a)
			{
				//check if a particular block has been removed
				if ((!platformerCharacter->GetBlockList()[a].isAlive && platformerCharacter->GetBlockList()[a].deathTimer == -1) && sceneNode->GetChildren()[a + localBindingData->nodeOffset]->GetSceneEntityCount() > 0)
				{
					SceneNode* blockNode = sceneNode->GetChildren()[a + localBindingData->nodeOffset];
					blockNode->RemoveModelEntity(blockNode->GetSceneEntity(0));
					blockNode->SetVisible(false);
				}
			}

			for (int a = 0; a < platformerCharacter->GetBlockCount(); ++a)
			{
				//check for blocks created since last frame
				if (platformerCharacter->GetBlockList()[a].createVersion > localBindingData->previousMeshVersion)
				{
					const int blockType = platformerCharacter->GetBlockList()[a].blockType;
					if (blockType >= 0 && platformerCharacter->GetBlockList()[a].isAlive)
					{
						SceneNode* node = new SceneNode(this->sceneGraph);
						Model* newModel = new Model(ResourceManager::StaticModels[ResourceManager::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(GameRenderer::SimpleVertexShader, GameRenderer::SimplePixelShader, GameRenderer::DefaultInputLayout));
						newModel->deleteBuffer = false;
						SceneEntity* newEntity = new SceneEntity(node, newModel);
						const BlockCoordinate& c = platformerCharacter->GetBlockList()[a];
						const XMVECTOR newLoc = XMVectorSet((c.x * blockUniverse->GetGridSpacing()), (c.y * blockUniverse->GetGridSpacing()), 0, 0);
						const XMVECTOR blockScale = XMVectorSet(blockUniverse->GetSquareWidth(), blockUniverse->GetSquareWidth(), blockUniverse->GetSquareWidth(), 1);
						node->AddSceneEntity(newEntity);
						node->SetTransformationVector(blockScale, objectData.rollPitchYaw, newLoc);
						sceneNode->AddChildNode(node);
						newModel->diffuseTint = blockColors[blockType];
						newModel->tint = blockEmissiveColors[blockType];
					}
				}
			}
		}
		localBindingData->previousMeshVersion = platformerCharacter->GetModificationVersion();
		//update animations
		const int blockCount = platformerCharacter->GetBlockCount();
		for (int a = 0; a < blockCount; ++a)
		{			
			if (platformerCharacter->GetBlockList()[a].isAlive/* &&
				platformerCharacter->GetBlockList()[a].animationStageCount > 0*/)
			{
				BlockCoordinate currentBlock = platformerCharacter->GetBlockList()[a];
				//if (localBindingData->lastAnimationStageIndex != currentBlock.animationStageIndex)
				{
					SceneNode* blockNode = sceneNode->GetChildren()[a + localBindingData->nodeOffset];
					const XMVECTOR newLoc = XMVectorSet((currentBlock.x * blockUniverse->GetGridSpacing()), (currentBlock.y * blockUniverse->GetGridSpacing()), 0, 0);
					const XMVECTOR blockScale = XMVectorSet(blockUniverse->GetSquareWidth(), blockUniverse->GetSquareWidth(), blockUniverse->GetSquareWidth(), 1);
					blockNode->SetTransformationVector(blockScale, objectData.rollPitchYaw, newLoc);
					localBindingData->lastAnimationStageIndex = currentBlock.animationStageIndex;
					if (blockNode->GetSceneEntityCount() > 0)
					{
						const int levelMultiplier = 1;
						
						blockNode->GetSceneEntity(0)->model->customValue = 
							XMVectorSet(currentBlock.blockLevel * levelMultiplier, 0, customValueCounterTest, 0);
					}
				}
			}
		}

		XMFLOAT4 loc;
		XMStoreFloat4(&loc, objectData.location);
		if (platformerCharacter->UseDiscreteAnimation())
		{
			loc.x = static_cast<int>((loc.x) / blockSize) * blockSize;
			loc.y = static_cast<int>((loc.y) / blockSize) * blockSize;
		}

		sceneNode->SetTransformationVector(XMVectorReplicate(1), objectData.rollPitchYaw, XMLoadFloat4(&loc));
		sceneNode->SetVelocity(objectData.object->GetVelocity());

		if (platformerCharacter->IlluminateVelocity())
		{
			XMVECTOR velMag = XMVector3Length(objectData.object->GetVelocity());
			velMag = XMVectorDivide(velMag, XMVectorReplicate(5));
			for (int a = 0; a < sceneNode->GetChildCount(); ++a)
			{
				if (sceneNode->GetChildren()[a]->GetSceneEntityCount() > 0)
				{
					sceneNode->GetChildren()[a]->GetSceneEntity(0)->model->tint = XMVectorMultiply(XMVectorSet(.7f, 0, 0, 1), velMag);
					sceneNode->GetChildren()[a]->GetSceneEntity(0)->model->diffuseTint = XMVectorSet(.5f, 0, 0, 1);
				}
			}
		}
	}

	void GameBindingGraph::updatePlatformerPersonCharacterSceneNode(SceneNode* sceneNode, const ActorData& objectData, LocalObjectData* localBindingData)
	{
		this->updatePlatformerCharacterSceneNode(sceneNode, objectData, localBindingData);

		//update the crosshair scene node

		const PlatformerPersonCharacter* const platformerCharacter = static_cast<const PlatformerPersonCharacter*>(objectData.object);

		int xOffset = 0;
		int yOffset = 0;

		for (int a = 0; a < platformerCharacter->GetBlockListByType(GUN).size(); ++a)
		{
			if (platformerCharacter->GetBlockList()[platformerCharacter->GetBlockListByType(GUN)[a]].isAlive && 
				platformerCharacter->GetBlockList()[platformerCharacter->GetBlockListByType(GUN)[a]].blockType == GUN)
			{
				xOffset = platformerCharacter->GetBlockList()[platformerCharacter->GetBlockListByType(GUN)[a]].x;
				yOffset = platformerCharacter->GetBlockList()[platformerCharacter->GetBlockListByType(GUN)[a]].y;
				//break;
			}
		}

		SceneNode* node = sceneNode->GetChildByName("crosshair");
		XMFLOAT4 loc;
		XMStoreFloat4(&loc, XMVector3Transform(XMVectorSet(150, 0, 0, 0), XMMatrixRotationZ(platformerCharacter->GetGunFacingDirection())));
		const float blockSize = platformerCharacter->GetParentBlockUniverse()->GetGridSpacing();

		loc.x = (static_cast<int>((loc.x + blockSize * .5f) / blockSize) + xOffset) * blockSize;
		loc.y = (static_cast<int>((loc.y + blockSize * .5f) / blockSize) + yOffset) * blockSize;
		node->SetTransformationVector(XMVectorSet(platformerCharacter->GetParentBlockUniverse()->GetSquareWidth(), platformerCharacter->GetParentBlockUniverse()->GetSquareWidth(), platformerCharacter->GetParentBlockUniverse()->GetSquareWidth(), 1), XMVectorSet(0, 0, 0, 0), XMLoadFloat4(&loc));
	}
}

