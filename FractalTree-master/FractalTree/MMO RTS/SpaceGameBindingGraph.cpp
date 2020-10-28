#include "pch.h"
#include "SpaceGameBindingGraph.h"
#include "UIRenderer.h"
#include "..\SpaceGame Common\Core\SpaceGameSimulation.h"
#include "..\SpaceGame Common\Core\SpaceGameResources.h"
#include "..\Common Source\Mesh\HeightMapMesh.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\SceneNode.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\SceneEntity.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Base\Direct3DRenderer.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\SceneGraph.h"

using namespace SimulationEngine;
using namespace RenderingEngine;
using namespace Geometry;
using namespace DirectX;

using namespace SpaceGameCore;
static int objectCountIndex = -1;
static int posIndex = -1;

namespace SpaceGameCore
{
	SpaceGameBindingGraph::SpaceGameBindingGraph(const SpaceGameSimulation* physicalWorldSimulation, SceneGraph* sceneGraph, const SpaceGameRenderer* renderer) :
		ObjectRendererBindingGraph<SpaceGameSimulation, ActorData, SpaceGameRenderer, LocalObjectData>(physicalWorldSimulation, sceneGraph, renderer)
	{

		if (objectCountIndex == -1)
		{
			objectCountIndex = UIRenderer::AddDebugString("Object count: ");
			posIndex = UIRenderer::AddDebugString("Player pos: ");
		}
		{
			SceneNode* const newNode = new SceneNode(this->sceneGraph);
			Model* const newModel = new Model(UIRenderer::uiModelPublic, renderer->CreateStandardModelShaderData(SpaceGameRenderer::UIVertexShader, SpaceGameRenderer::UIPixelShader, SpaceGameRenderer::UIInputLayout));
			newModel->deleteBuffer = false;
			SceneEntity* const newEntity = new SceneEntity(newNode, newModel);
			newNode->AddSceneEntity(newEntity);
			newNode->SetTransformationVector(XMVectorSet(.025f, .025f, 0, 1), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 0, 0, 0));
			this->mouseNode = newNode;
		}

		{
			SceneNode* const newNode = new SceneNode(this->sceneGraph);
			const ModelShaderData shaderData(this->renderer->GetVertexShader(SpaceGameRenderer::TerrainVertexShader),
				this->renderer->GetPixelShader(SpaceGameRenderer::SimpleMountainPixelShader),
				this->renderer->GetInputLayout(SpaceGameRenderer::TerrainInputLayout),
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				1,
				1,
				this->renderer->GetShaderResourceView(SpaceGameRenderer::MountainResourceView),
				this->renderer->GetSamplerState(SpaceGameRenderer::TerrainSamplerState),
				nullptr,
				nullptr,
				nullptr);
			const float width = 65;
			const float gridWidth = 32;
			
		}

	}

	RenderingEngine::SceneNode * SpaceGameBindingGraph::CreateSceneNodeFromObjectData(const Actor::ObjectData& objectData, LocalObjectData* localBindingData)
	{
		switch (objectData.objectType)
		{
		case ObjectTypes::Peasant:
			return this->createPeasantSceneNode(objectData);
		default:
			return this->createPeasantSceneNode(objectData);
		}
	}

	void SpaceGameBindingGraph::UpdateSceneNodeFromObjectData(RenderingEngine::SceneNode* sceneNode, const Actor::ObjectData& objectData, LocalObjectData* localBindingData)
	{
		sceneNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);
		sceneNode->SetVelocity(objectData.object->velocity);

		switch (objectData.objectType)
		{
		case ObjectTypes::Peasant:
			this->updatePeasantSceneNode(sceneNode, objectData, localBindingData);
			break;
			break;
		}
	}

	void SpaceGameBindingGraph::OnDeleteSceneNodeFromObjectData(RenderingEngine::SceneNode* sceneNode, const Actor::ObjectData& previousObjectData, LocalObjectData* localBindingData)
	{
	}

	void SpaceGameBindingGraph::UpdateRendererProperties()
	{
		const SpaceGamePlayer* const player = static_cast<SpaceGamePlayer*>(this->worldSimulation->GetPlayer(this->clientID));
		this->sceneGraph->SetCameraLocation(XMVectorAdd(this->worldSimulation->GetCameraLocation(this->clientID), XMVectorSet(0, 25, 0, 0))); //25
		const XMVECTOR facing = this->worldSimulation->GetCameraFacing(this->clientID);
		this->sceneGraph->SetCameraFacing(facing);
		//this->sceneGraph->SetCameraFacing
		//const XMVECTOR sideVec = XMVector3Normalize(XMVector3Cross(this->sceneGraph->GetCameraFacing(), XMVectorSet(0, 1, 0, 0)));
		//this->sceneGraph->SetCameraUp(XMVector3Normalize(XMVector3Cross(sideVec, this->sceneGraph->GetCameraFacing())));
		//this->sceneGraph->SetCameraUp(XMVector3Transform(XMVectorSet(0, 1, 0, 0), XMMatrixRotationRollPitchYawFromVector(this->worldSimulation->GetCameraPitchRollYa(0))));

		this->sceneGraph->SetCameraUp(this->worldSimulation->GetCameraUp(this->clientID));
		this->sceneGraph->SetCameraOrientation(this->worldSimulation->GetCameraOrientation(this->clientID));

		/*XMFLOAT4 newLoc;
		const XMVECTOR cameraLoc = this->worldSimulation->GetCameraLocation(player->GetGlobalID());
		XMStoreFloat4(&newLoc, cameraLoc);
		//newLoc.y = 2500;
		newLoc.z -= 125;
		newLoc.y += 50;
		this->sceneGraph->SetCameraLocation(XMLoadFloat4(&newLoc));
		this->sceneGraph->SetCameraFacing(-XMVector3Normalize(XMVectorSubtract(XMLoadFloat4(&newLoc), cameraLoc)));*/

		/*XMFLOAT4 newLoc;
		const XMVECTOR cameraLoc = this->worldSimulation->GetCameraLocation(player->GetGlobalID());
		XMStoreFloat4(&newLoc,cameraLoc);
		//newLoc.y = 2500;
		newLoc.z -= 125;
		newLoc.y += 50;
		this->sceneGraph->SetCameraLocation(XMLoadFloat4(&newLoc));
	//	this->sceneGraph->SetCameraFacing(-XMVector3Normalize(XMVectorSubtract(XMLoadFloat4(&newLoc), this->worldSimulation->GetCameraLocation(0))));

		this->sceneGraph->SetCameraUp(this->worldSimulation->GetCameraUp(this->clientID));
		this->sceneGraph->SetCameraOrientation(this->worldSimulation->GetCameraOrientation(this->clientID));
		/ *this->sceneGraph->SetCameraLocation(XMVectorSet(player->xCoord, 20, player->zCoord, 1));
		const XMVECTOR rot = XMVector3Normalize(XMVectorSet(.0f, -1, .75f, 0));//XMVector3Transform(XMVectorSet(0, -1, 0, 0), XMMatrixRotationX(anng));
		this->sceneGraph->SetCameraFacing(rot);

		const float x = this->worldSimulation->getPlayer(this->clientID)->mouseX;
		const float y = this->worldSimulation->getPlayer(this->clientID)->mouseY;
		this->mouseNode->SetTransformationVector(XMVectorSet(.025f, .025f, 0, 1), XMVectorSet(0, 0, 0, 0), XMVectorSet(x, y, 0, 0));* /*/
		UIRenderer::SetDebugString(objectCountIndex, "Object count: " + std::to_string(this->simulationObjectCount));
		UIRenderer::SetDebugString(posIndex, "Player pos: " + getVectorString(this->worldSimulation->GetCameraLocation(player->GetGlobalID())));
	}

	SceneNode* SpaceGameBindingGraph::createPeasantSceneNode(const ActorData& objectData)
	{
		SceneNode* const newNode = new SceneNode(this->sceneGraph);
		Model* const newModel = new Model(SpaceGameResources::StaticModels[SpaceGameResources::ResourceMap["cube2.m"]], renderer->CreateStandardModelShaderData(SpaceGameRenderer::SimpleVertexShader, SpaceGameRenderer::SimplePixelShader, SpaceGameRenderer::DefaultInputLayout));
		newModel->deleteBuffer = false;
		SceneEntity* const newEntity = new SceneEntity(newNode, newModel);
		newNode->AddSceneEntity(newEntity);
		newNode->SetTransformationVector(objectData.scale, objectData.rollPitchYaw, objectData.location);

		return newNode;
	}

	void SpaceGameBindingGraph::updatePeasantSceneNode(RenderingEngine::SceneNode* sceneNode, const ActorData& objectData, LocalObjectData* localBindingData)
	{
		/*const Peasant* const peasant = static_cast<const Peasant*>(objectData.object);
		if (peasant->Selected())
		{
			SceneNode* selectionNode = sceneNode->GetChildByName("selection_orb");
			if (selectionNode == nullptr)
			{
				selectionNode = new SceneNode(this->sceneGraph);
				Model* const newModel = new Model(SpaceGameResources::StaticModels[SpaceGameResources::ResourceMap["sphere.m"]], renderer->CreateStandardModelShaderData(SpaceGameRenderer::SimpleVertexShader, SpaceGameRenderer::SimplePixelShader, SpaceGameRenderer::DefaultInputLayout));
				newModel->deleteBuffer = false;
				SceneEntity* const newEntity = new SceneEntity(selectionNode, newModel);
				selectionNode->AddSceneEntity(newEntity);
				selectionNode->SetTransformationVector(XMVectorSet(.41, .41, .41, 1), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 1));
				sceneNode->AddChildNode(selectionNode, "selection_orb");
			}
		}
		else
		{
			SceneNode* const selectionNode = sceneNode->GetChildByName("selection_orb");
			if (selectionNode != nullptr)
			{
				selectionNode->Remove();
			}
		}*/
	}

	SpaceGameBindingGraph::~SpaceGameBindingGraph()
	{
		this->sceneGraph->Reset();
	}
}
