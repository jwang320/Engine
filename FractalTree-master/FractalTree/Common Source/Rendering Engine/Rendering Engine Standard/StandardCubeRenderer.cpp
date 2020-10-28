#include "pch.h"
#include "StandardCubeRenderer.h"
#include "..\Rendering Engine Standard\SceneGraph.h"
#include "..\Rendering Engine Standard\ModelVertexTypes.h"
#include "..\Rendering Engine Standard\SceneNode.h"
#include "..\Rendering Engine Standard\SceneEntity.h"
#include "..\Rendering Engine Standard\ParticleEntity.h"
#include "UIRenderer.h"
#include <fstream>
#include "..\Rendering Engine Standard\ParticleSystem.h"
#include "StandardRendererResources.h"
#include <string>
#include "..\Rendering Engine Base\Direct3DRenderer.h"

using namespace DirectX;

static int discardCount = 0;
static int discardIndex = 0;
static int facingDirIndex = 0;

namespace RenderingEngine
{
	static const float blendFactor[4] = {0.5f, 0.5f, 0.5f, 0.5f};
	static const UINT sampleMask   = 0xffffffff;
	static float ang = 0;

	StandardCubeRenderer::StandardCubeRenderer() :
		shaderDir("../x64/Shaders/")
	{
#ifndef _RTS_GAME
		discardIndex = UIRenderer::AddDebugString("Cell discard count: ");
#endif
		facingDirIndex = UIRenderer::AddDebugString("Camera Facing: ");
	}

#define USE_SHADOWSs
	void StandardCubeRenderer::Render()
	{
		this->makeRendererReady();
		this->updateCamera();
		this->updateLights();

#ifdef USE_SHADOWS
		//Build shadow map

		ID3D11ShaderResourceView * tab[1];
		tab[0]=NULL;
		this->deviceContext->PSSetShaderResources(10, 1, tab);
		ID3D11RenderTargetView* pnullView = NULL; 
		deviceContext->OMSetRenderTargets(1, &pnullView, pShadowMapDepthView);

		// Set the current viewport using the descriptor.
		this->deviceContext->RSSetViewports(1, &this->shadowViewPort);
		this->deviceContext->OMSetBlendState(this->blendStates[BlendOnState], blendFactor, sampleMask);
		this->deviceContext->OMSetDepthStencilState(this->depthStencilStates[DefaultDepthStencilState], 0);
		deviceContext->ClearDepthStencilView(pShadowMapDepthView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		this->renderModelsShadowMap();
		this->deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

		
		this->deviceContext->RSSetViewports(1, &this->windowViewPort);
		this->deviceContext->PSSetShaderResources(10, 1, &this->pShadowMapSRView);
#endif
		if(this->updateRasterizerState)
		{
			this->UpdateRasterizerState();
		}

		//If true, then render the entire scene first to a texture the size of the screen
		//This allows us to perform image processing effects on the texture
		//Then render the texture to a square overlaying the entire screen to show it.
		if(StandardRendererResources::renderPrePass)
		{
			this->renderSceneToTexture(this->prePassRenderTargetView, 11);
			this->renderSceneTextureToScreen(this->prePassShaderResourceView, 11, this->screenQuadModel);
		}
		else
		{
			//Render the scene directly to the screen with no pre-pass
			this->renderScene(this->renderTargetView);
		}
	}

	void StandardCubeRenderer::renderScene(ID3D11RenderTargetView* renderTarget)
	{	
		const float midnightBlue[] = {0, 0, 0, 0};
		this->deviceContext->ClearRenderTargetView(renderTarget, midnightBlue);
		this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		this->deviceContext->OMSetBlendState(this->blendStates[BlendOnState], blendFactor, sampleMask);
		this->deviceContext->OMSetDepthStencilState(this->depthStencilStates[DefaultDepthStencilState], 0);

		this->renderModels();
		this->renderParticles();
	}

	void StandardCubeRenderer::renderSceneToTexture(ID3D11RenderTargetView* renderTarget, int shaderResourceSlot)
	{
		ID3D11ShaderResourceView * tab[1];
		tab[0]=NULL;
		//Unbind resources
		this->deviceContext->PSSetShaderResources(shaderResourceSlot, 1, tab);
		ID3D11ShaderResourceView * tab2[1];
		tab2[0]=NULL;
		//Unbind resources
		this->deviceContext->PSSetShaderResources(12, 1, tab2);
		ID3D11ShaderResourceView * tab3[1];
		tab3[0]=NULL;
		//Unbind resources
		this->deviceContext->PSSetShaderResources(13, 1, tab3);

		ID3D11ShaderResourceView * tab4[1];
		tab4[0]=NULL;
		//Unbind resources
		this->deviceContext->PSSetShaderResources(14, 1, tab4);

		deviceContext->OMSetRenderTargets(1, &renderTarget, this->depthStencilView);
		this->renderScene(this->prePassRenderTargetView);
	}

	//Copy a scene texture rendered in the prepass to the screen and perform
	//post-processing
	void StandardCubeRenderer::renderSceneTextureToScreen(ID3D11ShaderResourceView* shaderResourceView, int shaderResourceSlot, Model* screenQuad)
	{
		const float midnightBlue[] = {0, 0, 0, 0};

		this->deviceContext->GenerateMips(this->prePassShaderResourceView);
		Direct3DRenderer::SetCurrentSamplerState(this->hdrSamplerState, 3);

		this->deviceContext->OMSetRenderTargets(1, &this->luminanceRenderTargetView, depthStencilView);
		this->deviceContext->PSSetShaderResources(shaderResourceSlot, 1, &shaderResourceView);
		Direct3DRenderer::RenderModel(screenQuad, this->hdrVertexShader, this->luminancePixelShader, screenQuad->modelShaderData.inputLayout);

		this->deviceContext->GenerateMips(this->luminanceShaderResourceView);
		this->deviceContext->OMSetRenderTargets(1, &this->bloomRenderTargetView, depthStencilView);
		this->deviceContext->PSSetShaderResources(14, 1, &this->luminanceShaderResourceView);
		//render to bloom target view
		this->deviceContext->ClearRenderTargetView(this->bloomRenderTargetView, midnightBlue);
		this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		this->deviceContext->OMSetDepthStencilState(this->depthStencilStates[DepthStencilStates::DepthOffState], 0);
		Direct3DRenderer::RenderModel(screenQuad, this->hdrVertexShader, this->hdrPixelShader, screenQuad->modelShaderData.inputLayout);

		//render to blurred texture
		this->deviceContext->OMSetRenderTargets(1, &this->blurRenderTargetView, depthStencilView);
		this->deviceContext->PSSetShaderResources(12, 1, &this->bloomShaderResourceView);
		this->deviceContext->ClearRenderTargetView(this->blurRenderTargetView, midnightBlue);
		this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		this->deviceContext->OMSetDepthStencilState(this->depthStencilStates[DepthStencilStates::DepthOffState], 0);
		Direct3DRenderer::RenderModel(screenQuad, this->hdrVertexShader, this->blurPixelShader, screenQuad->modelShaderData.inputLayout);

		//render to second blurred texture
		this->deviceContext->OMSetRenderTargets(1, &this->blurRenderTargetView2, depthStencilView);
		this->deviceContext->PSSetShaderResources(12, 1, &this->blurShaderResourceView);
		this->deviceContext->ClearRenderTargetView(this->blurRenderTargetView2, midnightBlue);
		this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		this->deviceContext->OMSetDepthStencilState(this->depthStencilStates[DepthStencilStates::DepthOffState], 0);
		Direct3DRenderer::RenderModel(screenQuad, this->hdrVertexShader, this->blurPixelShader, screenQuad->modelShaderData.inputLayout);

		//render to combined bloom
		this->deviceContext->OMSetRenderTargets(1, &this->renderTargetView, depthStencilView);
		this->deviceContext->PSSetShaderResources(13, 1, &this->blurShaderResourceView2);
		this->deviceContext->ClearRenderTargetView(this->renderTargetView, midnightBlue);
		this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		this->deviceContext->OMSetDepthStencilState(this->depthStencilStates[DepthStencilStates::DepthOffState], 0);
		Direct3DRenderer::RenderModel(screenQuad, this->hdrVertexShader, this->bloomCombinePixelShader, screenQuad->modelShaderData.inputLayout);

		this->deviceContext->OMSetDepthStencilState(this->depthStencilStates[DepthStencilStates::DefaultDepthStencilState], 0);
	}

	void StandardCubeRenderer::buildShadowMap()
	{
		ID3D11ShaderResourceView * tab[1];
		tab[0]=NULL;
		this->deviceContext->PSSetShaderResources(10, 1, tab);
		ID3D11RenderTargetView* pnullView = NULL; 
		deviceContext->OMSetRenderTargets(1, &pnullView, this->pShadowMapDepthView);

		// Set the current viewport using the descriptor.
		this->deviceContext->RSSetViewports(1, &this->shadowViewPort);
		this->deviceContext->OMSetBlendState(this->blendStates[BlendOnState], blendFactor, sampleMask);
		this->deviceContext->OMSetDepthStencilState(this->depthStencilStates[DefaultDepthStencilState], 0);
		deviceContext->ClearDepthStencilView(pShadowMapDepthView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		this->renderModelsShadowMap();
	}

	//Render all entities currently stored in the scene graph.
	void StandardCubeRenderer::renderModels()
	{
		discardCount = 0;
		this->oncePerModelBufferData.model = XMMatrixTranspose(XMMatrixIdentity());
		this->oncePerModelBufferData.scaleMatrix = XMMatrixTranspose(XMMatrixIdentity());
		this->oncePerModelBufferData.tint = XMVectorSet(0, 0, 0, 0);
		this->oncePerModelBufferData.customValue = XMVectorSet(0, 0, 0, 0);
		this->oncePerModelBufferData.diffuseTint = XMVectorSet(1, 1, 1, 1);
		this->deviceContext->UpdateSubresource(oncePerModelBuffer, 0, NULL, &oncePerModelBufferData, 0, 0);

		//Iterate through all nodes in scene graph linearly.
		for(unsigned int a = 0; a < this->sceneGraph->GetNodeCount(); ++a)
		{
			if (!this->sceneGraph->GetNode(a)->Removed() && this->sceneGraph->GetNode(a)->IsVisible())
			{
				this->oncePerModelBufferData.model = XMMatrixTranspose(this->sceneGraph->GetNode(a)->GetAbsoluteTransformation());

				//Render models.
				for (unsigned int entityIndex = 0; entityIndex < this->sceneGraph->GetNode(a)->GetSceneEntityCount(); ++entityIndex)
				{
					const bool testDepth = this->sceneGraph->GetNode(a)->GetSceneEntity(entityIndex)->enableZBuffer;
					if (!testDepth)
					{
						this->deviceContext->OMSetDepthStencilState(this->depthStencilStates[TransparencyDepthStencilState], NULL);
					}

					const Model* const entityModel = this->sceneGraph->GetNode(a)->GetSceneEntity(entityIndex)->GetModel();

					this->oncePerModelBufferData.tint = entityModel->tint;
					this->oncePerModelBufferData.diffuseTint = entityModel->diffuseTint;
					this->oncePerModelBufferData.customValue = entityModel->customValue;
					this->deviceContext->UpdateSubresource(this->oncePerModelBuffer, 0, NULL, &this->oncePerModelBufferData, 0, 0);
					if (!StandardRendererResources::CullCells || !entityModel->checkBounds ||
						Geometry::CheckProjectionCollisionBoundingBox(this->sceneGraph->GetCameraLocation(), this->oncePerFrameBufferData.view, this->oncePerResizeBufferData.projection,
						this->sceneGraph->GetNode(a)->GetSceneEntity(entityIndex)->boundingBox))
					{
						Direct3DRenderer::RenderModel(entityModel);
					}
					else
					{
						++discardCount;
					}

					if (!testDepth)
					{
						this->deviceContext->OMSetDepthStencilState(this->depthStencilStates[DefaultDepthStencilState], NULL);
					}
				}
			}
		}

#ifndef _RTS_GAME
		UIRenderer::SetDebugString(discardIndex, "Cell discard count: " + std::to_string(discardCount));
#endif
	}

	void StandardCubeRenderer::renderParticles()
	{
		this->deviceContext->OMSetBlendState(this->blendStates[BlendOffState], blendFactor, sampleMask);
		this->deviceContext->OMSetDepthStencilState(this->depthStencilStates[TransparencyDepthStencilState], 0);
		//Don't want to transform particle systems.
		this->oncePerModelBufferData.model = XMMatrixTranspose(XMMatrixIdentity());
		this->oncePerModelBufferData.tint = XMVectorSet(0, 0, 0, 0);
		this->oncePerModelBufferData.diffuseTint = XMVectorSet(1, 1, 1, 1);
		this->deviceContext->UpdateSubresource(oncePerModelBuffer, 0, NULL, &oncePerModelBufferData, 0, 0);

		for(unsigned int a = 0; a < this->sceneGraph->GetNodeCount(); ++a)
		{
			//Render particle systems.
			for(unsigned int particleEntityIndex = 0; particleEntityIndex < this->sceneGraph->GetNode(a)->GetParticleEntityCount(); ++particleEntityIndex)
			{
				Direct3DRenderer::RenderModel(this->sceneGraph->GetNode(a)->GetParticleEntity(particleEntityIndex)->model);					
			}
		}

		this->deviceContext->OMSetBlendState(this->blendStates[BlendOnState], blendFactor, sampleMask);
		this->deviceContext->OMSetDepthStencilState(this->depthStencilStates[DefaultDepthStencilState], 0);
	}

	void StandardCubeRenderer::renderModelsShadowMap()
	{
		this->oncePerModelBufferDataShadow.model = XMMatrixTranspose(XMMatrixIdentity());
		this->deviceContext->UpdateSubresource(this->oncePerModelBufferShadow, 0, NULL, &this->oncePerModelBufferDataShadow, 0, 0);

		//Iterate through all nodes in scene graph linearly.
		for(unsigned int a = 0; a < this->sceneGraph->GetNodeCount(); ++a)
		{
			this->oncePerModelBufferDataShadow.model = XMMatrixTranspose(this->sceneGraph->GetNode(a)->GetAbsoluteTransformation());

			this->deviceContext->UpdateSubresource(this->oncePerModelBufferShadow, 0, NULL, &this->oncePerModelBufferDataShadow, 0, 0);

			//Render models.
			for(unsigned int entityIndex = 0; entityIndex < this->sceneGraph->GetNode(a)->GetSceneEntityCount(); ++entityIndex)
			{
				const Model* const entityModel = this->sceneGraph->GetNode(a)->GetSceneEntity(entityIndex)->GetModel();

				if(entityModel->castsShadow)
				{
					if(!StandardRendererResources::CullCells || !entityModel->checkBounds ||
						Geometry::CheckProjectionCollisionBoundingBox(this->sceneGraph->GetCameraLocation(), this->oncePerFrameBufferData.view, this->oncePerResizeBufferData.projection,
						this->sceneGraph->GetNode(a)->GetSceneEntity(entityIndex)->boundingBox))
					{
						Direct3DRenderer::RenderModel(entityModel, this->shadowVertexShader, NULL, this->shadowInputLayout);
					}
				}
			}
		}
	}

	void StandardCubeRenderer::makeRendererReady()
	{
#ifdef _WIN8_1
		this->deviceContext->OMSetRenderTargets(1, &this->renderTargetView, this->depthStencilView);
#endif
	}

	void StandardCubeRenderer::updateCamera()
	{
		const XMVECTOR cameraFacing = this->sceneGraph->GetCameraFacing();
		XMFLOAT4 cameraF;
		XMStoreFloat4(&cameraF, this->sceneGraph->GetCameraOrientation());
		UIRenderer::SetDebugString(facingDirIndex, "Camera facing: (" + std::to_string(cameraF.x * 180 / 3.1415f) + ", " + std::to_string(cameraF.y * 180 / 3.1415f) + ", " + std::to_string(cameraF.z * 180 / 3.1415f) + ")");
		oncePerFrameBufferData.view = XMMatrixTranspose(XMMatrixLookAtLH(this->sceneGraph->GetCameraLocation(), 
			XMVectorAdd(this->sceneGraph->GetCameraLocation(), cameraFacing), this->sceneGraph->GetCameraUp()));
		oncePerFrameBufferData.eyePoint = this->sceneGraph->GetCameraLocation();
		oncePerFrameBufferData.cameraUp = this->sceneGraph->GetCameraUp();
	}

	void StandardCubeRenderer::updateLights()
	{
		this->oncePerFrameBufferData.light0Color =  this->sceneGraph->GetLightColor();
		this->oncePerFrameBufferData.light0Position = this->sceneGraph->GetLightPosition(); this->oncePerFrameBufferData.sunColor = this->sceneGraph->GetSunColor();// XMVectorSet(2, 2, 1, 1);
		this->oncePerFrameBufferData.light1Color = this->sceneGraph->GetLight1Color();
		this->oncePerFrameBufferData.light1Position = this->sceneGraph->GetLight1Position();
		this->oncePerFrameBufferData.customValue = this->sceneGraph->GetCustomValue();
		const XMVECTOR sunPosition = this->sceneGraph->GetSunPosition();
		const XMVECTOR sunDirection = XMVector3Normalize(-sunPosition);
		this->oncePerFrameBufferData.sunDirection = -sunDirection;	
		
		const XMMATRIX lightView = XMMatrixLookAtRH(XMVectorAdd(this->sceneGraph->GetCameraLocation(),sunPosition), this->sceneGraph->GetCameraLocation(), XMVectorSet(0, 1, 0, 0));
		const XMMATRIX lightProj = XMMatrixOrthographicRH(shadowWidth, shadowWidth, 100,50000);
		
		this->oncePerFrameBufferData.lightProjection = XMMatrixTranspose(lightProj);
		this->oncePerFrameBufferData.lightView = XMMatrixTranspose(lightView);

		if(StandardRendererResources::rotateSun)
			ang+=.05f;
		if(ang > 360)
			ang = 0;
		this->deviceContext->UpdateSubresource(this->oncePerFrameBuffer, 0, NULL, &this->oncePerFrameBufferData, 0, 0);
	}

	void StandardCubeRenderer::UpdateRasterizerState()
	{
		this->updateRasterizerState = false;
		if(this->wireFrame)
		{
			this->deviceContext->RSSetState(this->rasterizerStates[WireframeRasterizerState]);
		}
		else
		{
			this->deviceContext->RSSetState(this->rasterizerStates[DefaultRasterizerState]);
		}
	}

	ModelShaderData StandardCubeRenderer::CreateStandardModelShaderData(int vertexShaderID, int pixelShaderID, int inputLayoutID, int textureSlot, int shaderResourceID, int samplerStateID, int shaderResourceSlot, int samplerStateSlot) const
	{
		ModelShaderData shaderData(this->vertexShaders[vertexShaderID], this->pixelShaders[pixelShaderID], this->inputLayouts[inputLayoutID], D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			shaderResourceSlot,
			samplerStateSlot);
		if(textureSlot > 0)
		{
			shaderData.textureSamplerState = this->samplerStates[samplerStateID];
			shaderData.textureResourceView = this->shaderResourceViews[shaderResourceID];
		}
		else
		{
			shaderData.textureSamplerState = NULL;
			shaderData.textureResourceView = NULL;
		}

		shaderData.alphaMap = NULL;
		shaderData.alphaMapTexture = NULL;

		return shaderData;
	}

	ModelShaderData StandardCubeRenderer::CreateStandardParticleShaderData(int vertexShaderID, int pixelShaderID, int inputLayoutID, int textureSlot, int shaderResourceID, int samplerStateID, int shaderResourceSlot, int samplerStateSlot) const
	{
		ModelShaderData shaderData(this->vertexShaders[vertexShaderID], this->pixelShaders[pixelShaderID]);
		shaderData.inputLayout = this->inputLayouts[inputLayoutID];
		shaderData.primitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		shaderData.samplerStateSlot = samplerStateSlot;
		shaderData.resourceViewSlot = shaderResourceSlot;
		if(textureSlot > 0)
		{
			shaderData.textureSamplerState = this->samplerStates[samplerStateID];
			shaderData.textureResourceView = this->shaderResourceViews[shaderResourceID];
		}
		else
		{
			shaderData.textureSamplerState = NULL;
			shaderData.textureResourceView = NULL;
			shaderData.alphaMap = NULL;
		}

		return shaderData;
	}

	void StandardCubeRenderer::ToggleWireFrame()
	{
		this->wireFrame = !this->wireFrame;
		this->updateRasterizerState = true;
	}

	StandardCubeRenderer::~StandardCubeRenderer()
	{
		if(this->oncePerFrameBuffer != NULL)
			this->oncePerFrameBuffer->Release();
		if(this->oncePerModelBuffer != NULL)
			this->oncePerModelBuffer->Release();
		if(this->oncePerResizeBuffer != NULL)
			this->oncePerResizeBuffer->Release();
		
		if(this->pShadowMap != NULL)
			this->pShadowMap->Release();
		if(this->pShadowMapDepthView != NULL)
			this->pShadowMapDepthView->Release();
		if(this->pShadowMapSRView != NULL)
			this->pShadowMapSRView->Release();
		if(this->shadowPixelShader != NULL)
			this->shadowPixelShader->Release();
		if(this->shadowVertexShader != NULL)
			this->shadowVertexShader->Release();
		if(this->shadowInputLayout != NULL)
			this->shadowInputLayout->Release();
		if(this->shadowSamplerState != NULL)
			this->shadowSamplerState->Release();
		if(this->shadowRasterizerState != NULL)
			this->shadowRasterizerState->Release();
		if(this->oncePerModelBufferShadow != NULL)
			this->oncePerModelBufferShadow->Release();
	}
}