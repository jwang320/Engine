#include "pch.h"
#include "StandardCubeRenderer.h"
#include "..\Rendering Engine Standard\SceneGraph.h"
#include "..\Common Source\Util\File Parser\ASCII\ParseHelper.h"

using namespace DirectX;

namespace RenderingEngine
{
	void StandardCubeRenderer::InitializeOther()
	{
		this->sceneGraph = new SceneGraph;

		this->InitializeProjection();
		this->initializeDepthStencilStates();
		this->initializeBlendStates();
		this->initializeRenderTargetsTemp();
#ifndef _WIN8_1
#ifdef _WIN32_APPLICATION
		if (useHDR)
			this->initializeHDR();
#endif
#endif
	}

	void StandardCubeRenderer::ResetSceneGraph()
	{
		if (this->sceneGraph != NULL)
		{
			delete this->sceneGraph;
		}

		this->sceneGraph = new SceneGraph();
	}

	bool StandardCubeRenderer::initializeRenderTargetsTemp()
	{
		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = static_cast<UINT>(shadowWidth);
		texDesc.Height = static_cast<UINT>(shadowWidth);
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		texDesc.SampleDesc.Count = 1;//this->sampleCount;
		texDesc.SampleDesc.Quality = 0;//this->sampleQuality;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		// Create the depth stencil view desc
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		//create shader resource view desc
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;

		//create texture and depth/resource views
		if( FAILED( device->CreateTexture2D( &texDesc, NULL, &pShadowMap ) ) )  return false;
		if( FAILED( device->CreateDepthStencilView( pShadowMap, &descDSV, &pShadowMapDepthView ) ) ) return false;
		if( FAILED( device->CreateShaderResourceView( pShadowMap, &srvDesc, &pShadowMapSRView) ) ) return false;

		ZeroMemory(&this->shadowViewPort, sizeof(D3D11_VIEWPORT));
		this->shadowViewPort.TopLeftX = 0;
		this->shadowViewPort.TopLeftY = 0;
		this->shadowViewPort.MinDepth = 0.0f;
		this->shadowViewPort.MaxDepth = 1.0f;
		this->shadowViewPort.Width = static_cast<float>(this->shadowWidth);
		this->shadowViewPort.Height = static_cast<float>(this->shadowWidth);

		

		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));

		samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		// Create the texture sampler state.
		HRESULT hr = device->CreateSamplerState(&samplerDesc, &this->shadowSamplerState);
		if(FAILED(hr))
		{
			throw int();
		}

		this->deviceContext->PSSetSamplers(2,1, &this->shadowSamplerState);

		D3D11_RASTERIZER_DESC rState2 =
		{
			D3D11_FILL_SOLID,
			D3D11_CULL_FRONT,
			true,
			0,
			0.f,
			0.f,
			true,
			false,
			false,
			false
		};

		if(FAILED(this->device->CreateRasterizerState(&rState2, &this->shadowRasterizerState)))
		{
			throw int();
		}
		//this->deviceContext->RSSetState(this->rasterizerStates[DefaultRasterizerState]);

#ifndef _WIN8_1
	//	this->initializeHDR();
#endif
		return true;
	}

	struct SimpleV
	{
		XMVECTOR vert;
		XMFLOAT2 tx;
	};
	void StandardCubeRenderer::initializeHDR()
	{
		HRESULT hr;
		{
			D3D11_TEXTURE2D_DESC texDesc;
			ZeroMemory(&texDesc, sizeof(texDesc));
			texDesc.Width = static_cast<UINT>(this->width);
			texDesc.Height = static_cast<UINT>(this->height);
			texDesc.MipLevels = 11;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

			//create shader resource view desc
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;

			D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
			ZeroMemory(&viewDesc, sizeof(viewDesc));
			viewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			//create texture and depth/resource views
			if (FAILED(device->CreateTexture2D(&texDesc, NULL, &this->hdrTexture)))
				throw int();
			if (FAILED(device->CreateShaderResourceView(this->hdrTexture, &srvDesc, &this->prePassShaderResourceView)))
				throw int();
			if (FAILED(device->CreateRenderTargetView(this->hdrTexture, &viewDesc, &this->prePassRenderTargetView)))
				throw int();

			D3D11_SAMPLER_DESC samplerDesc;
			ZeroMemory(&samplerDesc, sizeof(samplerDesc));

			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MipLODBias = 0;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerDesc.BorderColor[0] = 0;
			samplerDesc.BorderColor[1] = 0;
			samplerDesc.BorderColor[2] = 0;
			samplerDesc.BorderColor[3] = 0;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			// Create the texture sampler state.
			hr = device->CreateSamplerState(&samplerDesc, &this->hdrSamplerState);
			if(FAILED(hr))
			{
				throw int();
			}
		}

		{
			D3D11_TEXTURE2D_DESC texDesc;
			ZeroMemory(&texDesc, sizeof(texDesc));
			texDesc.Width = static_cast<UINT>(this->width);
			texDesc.Height = static_cast<UINT>(this->height);
			texDesc.MipLevels = 11;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

			//create shader resource view desc
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;

			D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
			ZeroMemory(&viewDesc, sizeof(viewDesc));
			viewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			//create texture and depth/resource views
			if (FAILED(device->CreateTexture2D(&texDesc, NULL, &this->luminanceTexture)))
				throw int();
			if (FAILED(device->CreateShaderResourceView(this->luminanceTexture, &srvDesc, &this->luminanceShaderResourceView)))
				throw int();
			if (FAILED(device->CreateRenderTargetView(this->luminanceTexture, &viewDesc, &this->luminanceRenderTargetView)))
				throw int();

			
		}

		{
			D3D11_TEXTURE2D_DESC texDesc;
			ZeroMemory(&texDesc, sizeof(texDesc));
			texDesc.Width = static_cast<UINT>(this->width);
			texDesc.Height = static_cast<UINT>(this->height);
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			//create shader resource view desc
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;

			D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
			ZeroMemory(&viewDesc, sizeof(viewDesc));
			viewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			//create texture and depth/resource views
			if (FAILED(device->CreateTexture2D(&texDesc, NULL, &this->bloomTexture)))
				throw int();
			if (FAILED(device->CreateShaderResourceView(this->bloomTexture, &srvDesc, &this->bloomShaderResourceView)))
				throw int();
			if (FAILED(device->CreateRenderTargetView(this->bloomTexture, &viewDesc, &this->bloomRenderTargetView)))
				throw int();

			D3D11_SAMPLER_DESC samplerDesc;
			ZeroMemory(&samplerDesc, sizeof(samplerDesc));

			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerDesc.BorderColor[0] = 0;
			samplerDesc.BorderColor[1] = 0;
			samplerDesc.BorderColor[2] = 0;
			samplerDesc.BorderColor[3] = 0;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			// Create the texture sampler state.
			hr = device->CreateSamplerState(&samplerDesc, &this->bloomSamplerState);
			if(FAILED(hr))
			{
				throw int();
			}
		}

		{
			D3D11_TEXTURE2D_DESC texDesc;
			ZeroMemory(&texDesc, sizeof(texDesc));
			texDesc.Width = static_cast<UINT>(this->width);
			texDesc.Height = static_cast<UINT>(this->height);
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			//create shader resource view desc
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;

			D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
			ZeroMemory(&viewDesc, sizeof(viewDesc));
			viewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			//create texture and depth/resource views
			if (FAILED(device->CreateTexture2D(&texDesc, NULL, &this->blurTexture)))
				throw int();
			if (FAILED(device->CreateShaderResourceView(this->blurTexture, &srvDesc, &this->blurShaderResourceView)))
				throw int();
			if (FAILED(device->CreateRenderTargetView(this->blurTexture, &viewDesc, &this->blurRenderTargetView)))
				throw int();

			
		}

		{
			D3D11_TEXTURE2D_DESC texDesc;
			ZeroMemory(&texDesc, sizeof(texDesc));
			texDesc.Width = static_cast<UINT>(this->width);
			texDesc.Height = static_cast<UINT>(this->height);
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			//create shader resource view desc
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;

			D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
			ZeroMemory(&viewDesc, sizeof(viewDesc));
			viewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			//create texture and depth/resource views
			if (FAILED(device->CreateTexture2D(&texDesc, NULL, &this->blurTexture2)))
				throw int();
			if (FAILED(device->CreateShaderResourceView(this->blurTexture2, &srvDesc, &this->blurShaderResourceView2)))
				throw int();
			if (FAILED(device->CreateRenderTargetView(this->blurTexture2, &viewDesc, &this->blurRenderTargetView2)))
				throw int();

			
		}

		{
			hr;
#ifndef _WIN8_1
			std::ifstream myFile(this->shaderDir + "HDRVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);
#else
			std::ifstream myFile("HDRVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);
#endif
			if(myFile.is_open())
			{
				const size_t fileSize = myFile.tellg();
				myFile.seekg(0, std::ios::beg);
				char* shaderData = new char[fileSize];
				myFile.read(shaderData, fileSize);
				myFile.close();
				hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->hdrVertexShader);
				if (FAILED(hr)) { delete[] shaderData; throw int(); }

				const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				hr = this->device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, fileSize, &this->hdrInputLayout);

				delete[] shaderData;

				if (FAILED(hr)) {
					throw int();
				}
			}
		}

		{
			size_t pixelShaderSize;
#ifndef _WIN8_1
			char* pixelShaderData = GetFileDataAndSize2(std::string(this->shaderDir + "HDRPixelShader.cso").c_str(), pixelShaderSize);
#else
			char* pixelShaderData = GetFileDataAndSize2("HDRPixelShader.cso", pixelShaderSize);
#endif
			hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->hdrPixelShader);
			delete[] pixelShaderData;
			if(FAILED(hr))
			{
				throw int();
			}
		}
		{
			size_t pixelShaderSize;

#ifndef _WIN8_1
			char* pixelShaderData = GetFileDataAndSize2(std::string(this->shaderDir + "LuminancePixelShader.cso").c_str(), pixelShaderSize);
#else
			char* pixelShaderData = GetFileDataAndSize2("LuminancePixelShader.cso", pixelShaderSize);
#endif
			hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->luminancePixelShader);
			delete[] pixelShaderData;
			if(FAILED(hr))
			{
				throw int();
			}
		}
		const float qW = 1;
		SimpleV vertices[4] = {{XMVectorSet(-qW, -qW, 0, 1), XMFLOAT2(0, 1)},
		{XMVectorSet(qW, -qW, 0, 1), XMFLOAT2(1, 1)},
		{XMVectorSet(qW, qW, 0, 1), XMFLOAT2(1, 0)},
		{XMVectorSet(-qW, qW, 0, 1), XMFLOAT2(0, 0)}};

		shaderindextype indices[6] = {0, 1, 2, 0, 2, 3};
		ModelBufferData* bufferData = new ModelBufferData();
		bufferData->indexCount[0] = 6;
		bufferData->vertexCount = 4;
		bufferData->indexFormat = DEFAULT_INDEX_FORMAT;

		bufferData->vertexSize = sizeof(SimpleV);
		bufferData->indexOffset = 0;
		bufferData->vertexOffset = 0;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(SimpleV) * bufferData->vertexCount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;


		D3D11_SUBRESOURCE_DATA InitDataV;
		InitDataV.pSysMem = vertices;
		InitDataV.SysMemPitch = 0;
		InitDataV.SysMemSlicePitch = 0;
		if(FAILED(device->CreateBuffer(&bd, &InitDataV, &bufferData->vertexBuffer)))      // create the buffer
		{
			throw int();
		}

		// Fill in a buffer description.
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage           = D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth = sizeof(shaderindextype) * bufferData->indexCount[0];
		bufferDesc.BindFlags       = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags  = 0;
		bufferDesc.MiscFlags       = 0;

		// Define the resource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		// Create the buffer with the device.
		hr = device->CreateBuffer(&bufferDesc, &InitData, &bufferData->indexBuffer[0]);
		if (FAILED(hr))
			throw int();

		ModelShaderData screenShaderData(this->hdrVertexShader, this->hdrPixelShader, this->hdrInputLayout);
		screenShaderData.textureSamplerState = this->hdrSamplerState;
		screenShaderData.primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		screenShaderData.samplerStateSlot = 3;
		this->screenQuadModel = new Model(bufferData, screenShaderData);
	}

	void StandardCubeRenderer::InitializeBuffers()
	{
		this->device->CreateBuffer(
			&CD3D11_BUFFER_DESC(sizeof(OncePerFrameConstantBuffer), D3D11_BIND_CONSTANT_BUFFER),
			nullptr,
			&this->oncePerFrameBuffer);
		this->device->CreateBuffer(
			&CD3D11_BUFFER_DESC(sizeof(OncePerModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER),
			nullptr,
			&this->oncePerModelBuffer);
		this->device->CreateBuffer(
			&CD3D11_BUFFER_DESC(sizeof(OncePerResizeConstantBuffer), D3D11_BIND_CONSTANT_BUFFER),
			nullptr,
			&this->oncePerResizeBuffer);
		this->device->CreateBuffer(
			&CD3D11_BUFFER_DESC(sizeof(OncePerModelConstantBufferShadow), D3D11_BIND_CONSTANT_BUFFER),
			nullptr,
			&this->oncePerModelBufferShadow);

		this->deviceContext->VSSetConstantBuffers(0, 1, &this->oncePerFrameBuffer);
		this->deviceContext->VSSetConstantBuffers(1, 1, &this->oncePerModelBuffer);
		this->deviceContext->VSSetConstantBuffers(2, 1, &this->oncePerResizeBuffer);
		this->deviceContext->VSSetConstantBuffers(3, 1, &this->oncePerModelBufferShadow);
		this->deviceContext->PSSetConstantBuffers(0, 1, &this->oncePerFrameBuffer);
	}


	void StandardCubeRenderer::InitializeProjection()
	{
		this->aspectRatio = static_cast<float>(this->width) / this->height;

		this->fovAngleY = 70.0f * XM_PI / 180.0f;
		if(this->aspectRatio < 1.0f)
		{
			this->fovAngleY /= this->aspectRatio;
		}

		if(this->featureLevel == D3D_FEATURE_LEVEL_9_1 || this->featureLevel == D3D_FEATURE_LEVEL_9_1 || this->featureLevel == D3D_FEATURE_LEVEL_9_3)
		{
			this->nearZPlane = 100.01f;
		}
		else
		{
#ifdef _RTS_GAME
			this->nearZPlane = 10.01f;
#else
			this->nearZPlane = FractalTreeResources::nearZDistance;
#endif
		}

		const XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(
			fovAngleY,
			aspectRatio,
			nearZPlane,
#ifdef _RTS_GAME
			200000);
#else
			FractalTreeResources::farZDistance);//FractalTreeResources::MaxViewDistance);
#endif
		this->oncePerResizeBufferData.projection = XMMatrixTranspose(projectionMatrix);

		this->sceneGraph->SetProjectionMatrix(projectionMatrix);
		this->deviceContext->UpdateSubresource(this->oncePerResizeBuffer, 0, NULL, &this->oncePerResizeBufferData, 0, 0);
	}

	void StandardCubeRenderer::initializeBlendStates()
	{
		D3D11_BLEND_DESC BlendState;
		ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
		BlendState.AlphaToCoverageEnable = TRUE;
		BlendState.RenderTarget[0].BlendEnable = TRUE;
		BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendState.RenderTarget[0].RenderTargetWriteMask = 0x0f;
		this->device->CreateBlendState(&BlendState, &this->blendStates[BlendOnState]);

		//this->deviceContext->OMSetBlendState(this->blendStates[BlendOnState], blendFactor, sampleMask);		

		D3D11_BLEND_DESC BlendState2;
		ZeroMemory(&BlendState2, sizeof(D3D11_BLEND_DESC));
		BlendState2.AlphaToCoverageEnable = FALSE;
		BlendState2.RenderTarget[0].BlendEnable = TRUE;
		BlendState2.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		BlendState2.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BlendState2.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BlendState2.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendState2.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		BlendState2.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendState2.RenderTarget[0].RenderTargetWriteMask = 0x0f;
		this->device->CreateBlendState(&BlendState2, &this->blendStates[BlendOffState]);
	}

	void StandardCubeRenderer::initializeDepthStencilStates()
	{
		this->depthStencilStates.resize(3);
		this->deviceContext->OMGetDepthStencilState(&this->depthStencilStates[DefaultDepthStencilState], NULL);

		D3D11_DEPTH_STENCIL_DESC stencilDesc;
		stencilDesc.DepthEnable = true;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		stencilDesc.StencilEnable = false;
		stencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		stencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

		this->device->CreateDepthStencilState(&stencilDesc, &this->depthStencilStates[TransparencyDepthStencilState]);

		D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
		depthDisabledStencilDesc.DepthEnable = false;
		depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthDisabledStencilDesc.StencilEnable = true;
		depthDisabledStencilDesc.StencilReadMask = 0xFF;
		depthDisabledStencilDesc.StencilWriteMask = 0xFF;
		depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		this->device->CreateDepthStencilState(&depthDisabledStencilDesc, &this->depthStencilStates[DepthOffState]);
	}
}