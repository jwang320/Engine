#include "pch.h"
#include "FractalTreeRenderer.h"
#include "UIRenderer.h"
#include <synchapi.h>
#include "BasicReaderWriter.h"
#include "..\Common Source\Util\Texture Loader\DDSTextureLoader.h"

using namespace DirectX;
using namespace Geometry;

namespace RenderingEngine
{
	FractalTreeRenderer::FractalTreeRenderer()
	{
		FractalTreeResources::terrainTextureRepeatCount = 1 << this->terrainTextureRepeatIndex;
	}

#pragma region Initialization
	void FractalTreeRenderer::InitializeOther()
	{
		__super::InitializeOther();
		this->initializeModels();
		HRESULT hr = this->device->CreateBuffer(
			&CD3D11_BUFFER_DESC(sizeof(PerlinBuffer), D3D11_BIND_CONSTANT_BUFFER),
			nullptr,
			&this->perlinBuffer);
		this->initializeUI(FontSamplerState, UIPixelShader, UIVertexShader, UIInputLayout, FontTexture, FontPixelShader, FontResourceView);

		this->deviceContext->PSSetConstantBuffers(3, 1, &this->perlinBuffer);
		this->perlinBufferData.terrainTextureRepeatCount[0] = FractalTreeResources::terrainTextureRepeatCount;
		this->perlinBufferData.terrainTextureRepeatCount[1] = this->perlinAmplitude;
		this->perlinBufferData.terrainTextureRepeatCount[2] = FractalTreeResources::terrainTextureRepeatCount;
		this->perlinBufferData.terrainTextureRepeatCount[3] = FractalTreeResources::terrainTextureRepeatCount;
		this->deviceContext->UpdateSubresource(this->perlinBuffer, 0, NULL, &this->perlinBufferData, 0, 0);
	}

	bool FractalTreeRenderer::initializeModels()
	{
		FractalTreeResources::LoadResources(this);
		return true;
	}

	void FractalTreeRenderer::InitializeShaders()
	{
		this->vertexShaders.resize(8);
		this->pixelShaders.resize(12);
		this->inputLayouts.resize(8);

		this->initializeSimpleVertexShader();
		this->initializeFontVertexShader();
		this->initializeWorldFontVertexShader();
		//this->initializeParticleVertexShader();
		this->initializeSimplePixelShader();
		this->initializeSimpleMountainPixelShader();
		this->initializeUIPixelShader();
		this->initializeFontPixelShader();
		this->initializeTreePixelShader();
		this->initializeParticlePixelShader();
		this->initializeTerrainVertexShader();
		this->initializeInstancedFoliageVertexShader();
		this->initializeFoliagePixelShader();
		this->initializeWireFramePixelShader();
		this->initializeWireFrameVertexShader();
		this->initializeSkyVertexShader();
		this->initializeSkyPixelShader();
		this->initializeWaterPixelShader();
		this->initializeBlurShader();
		this->initializeBloomCombineShader();
	}

	void FractalTreeRenderer::InitializeTextures()
	{
		this->samplerStates.resize(5);
		this->textures.resize(12);
		this->shaderResourceViews.resize(11);
		this->blendStates.resize(2);

		HRESULT hr = CreateDDSTextureFromFile(this->device,
			L"../Resources/Textures/dirt2.dds",
			&this->textures[MountainTexture],
			&this->shaderResourceViews[MountainResourceView]);

		hr = CreateDDSTextureFromFile(this->device,
			L"../Resources/Textures/grass2.dds",
			&this->textures[GrassTexture],
			&this->shaderResourceViews[GrassResourceView]);

		hr = CreateDDSTextureFromFile(this->device,
			L"../Resources/Textures/Stone_DarkGray.dds",
			&this->textures[SlabTexture],
			&this->shaderResourceViews[SlabResourceView]);

		hr = CreateDDSTextureFromFile(this->device,
			L"../Resources/Textures/stone_1_2048x2048.dds",
			&this->textures[StoneTexture],
			&this->shaderResourceViews[StoneResourceView]);

		hr = CreateDDSTextureFromFile(this->device,
			L"../Resources/Textures/displacement.dds",
			&this->textures[BumpMapTexture],
			&this->shaderResourceViews[BumpmapResourceView]);

		/*hr = CreateDDSTextureFromFile(this->device,
			L"../Resources/Textures/SnowIceTexture0003.dds",
			&this->textures[IceTexture],
			&this->shaderResourceViews[IceResourceView]);*/

		/*hr = CreateDDSTextureFromFile(this->device,
			L"../Resources/Textures/corgifullb.dds",
			&this->textures[ExplosionTexture],
			&this->shaderResourceViews[ExplosionResourceView]);*/

		/*hr = CreateDDSTextureFromFile(this->device,
			L"../Resources/Textures/grasswip1.dds",
			&this->textures[LeafTexture1],
			&this->shaderResourceViews[LeafTexture1ResourceView]);*/

		/*hr = CreateDDSTextureFromFile(this->device,
			L"../Resources/Textures/Leaf_1_Normal.dds",
			&this->textures[LeafTextureNormal1],
			&this->shaderResourceViews[LeafTextureNormal1ResourceView]);*/

		/*hr = CreateDDSTextureFromFile(this->device,
			L"../Resources/Textures/organtexture.dds",
			&this->textures[OrganTexture],
			&this->shaderResourceViews[OrganTextureResourceView]);*/
		
		this->deviceContext->PSSetShaderResources(2, 1, &this->shaderResourceViews[GrassResourceView]);
		this->deviceContext->PSSetShaderResources(4, 1, &this->shaderResourceViews[SlabResourceView]);
		this->deviceContext->PSSetShaderResources(5, 1, &this->shaderResourceViews[StoneResourceView]);
		this->deviceContext->PSSetShaderResources(6, 1, &this->shaderResourceViews[BumpmapResourceView]);
		//this->deviceContext->PSSetShaderResources(7, 1, &this->shaderResourceViews[IceResourceView]);
		//this->deviceContext->PSSetShaderResources(8, 1, &this->shaderResourceViews[ExplosionResourceView]);
		//this->deviceContext->PSSetShaderResources(9, 1, &this->shaderResourceViews[LeafTextureNormal1ResourceView]);

		D3D11_SAMPLER_DESC samplerDesc2;
		//samplerDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc2.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc2.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc2.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc2.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc2.MipLODBias = 0;//0.0f;
		samplerDesc2.MaxAnisotropy = 16;
		samplerDesc2.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc2.BorderColor[0] = 1;
		samplerDesc2.BorderColor[1] = 1;
		samplerDesc2.BorderColor[2] = 1;
		samplerDesc2.BorderColor[3] = 1;
		samplerDesc2.MinLOD = -D3D11_FLOAT32_MAX;
		samplerDesc2.MaxLOD = D3D11_FLOAT32_MAX;;

		// Create the texture sampler state.
		if(FAILED(this->device->CreateSamplerState(&samplerDesc2, &this->samplerStates[TerrainSamplerState])))
		{
			exit(22);
		}

	}

	
#pragma endregion

	int FractalTreeRenderer::IncreaseTextureRepeatCount()
	{
		++this->terrainTextureRepeatIndex;
		FractalTreeResources::terrainTextureRepeatCount = 1 << this->terrainTextureRepeatIndex;
		this->perlinBufferData.terrainTextureRepeatCount[0] = FractalTreeResources::terrainTextureRepeatCount;
		this->deviceContext->UpdateSubresource(this->perlinBuffer, 0, NULL, &this->perlinBufferData, 0, 0);
		return FractalTreeResources::terrainTextureRepeatCount;
	}

	int FractalTreeRenderer::DecreaseTextureRepeatCount()
	{
		--this->terrainTextureRepeatIndex;
		FractalTreeResources::terrainTextureRepeatCount = 1 << this->terrainTextureRepeatIndex;
		this->perlinBufferData.terrainTextureRepeatCount[0] = FractalTreeResources::terrainTextureRepeatCount;
		this->deviceContext->UpdateSubresource(this->perlinBuffer, 0, NULL, &this->perlinBufferData, 0, 0);
		return FractalTreeResources::terrainTextureRepeatCount;
	}

	float FractalTreeRenderer::ChangePerlinAmplitude(float delta)
	{
		if(delta < 0)
		{
			this->perlinAmplitude -= .3f;
		}
		else
		{
			this->perlinAmplitude += .3f;
		}

		this->perlinBufferData.terrainTextureRepeatCount[1] = this->perlinAmplitude;
		this->deviceContext->UpdateSubresource(this->perlinBuffer, 0, NULL, &this->perlinBufferData, 0, 0);
		return this->perlinAmplitude;
	}

	void FractalTreeRenderer::CreateTextureFromArray(unsigned int* colors, unsigned int size, TextureData* textureDataResult) const
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = size;
		textureDesc.Height = size;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Format =  DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Usage = D3D11_USAGE_DYNAMIC;

		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//ID3D11Texture2D* tex = NULL;
		//textureDataResult->texture = tex;
		if(FAILED(this->device->CreateTexture2D(&textureDesc, NULL, &textureDataResult->texture)))
		{
			exit(22);
		}


		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
		srDesc.Format = textureDesc.Format;
		srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc.Texture2D.MostDetailedMip = 0;
		srDesc.Texture2D.MipLevels = 1;

		if(FAILED(this->device->CreateShaderResourceView(textureDataResult->texture, &srDesc, &textureDataResult->textureResourceView)))
		{
			exit(22);
		}

		D3D11_MAPPED_SUBRESOURCE ms;
		this->deviceContext->Map(textureDataResult->texture, 0, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

		memcpy(ms.pData, colors, sizeof(unsigned int) * size * size);
		this->deviceContext->Unmap(textureDataResult->texture, 0);
		//tex->Release();
	}

	void FractalTreeRenderer::UpdateTextureFromArray(unsigned int* colors, unsigned int size, TextureData* textureData) const
	{
		D3D11_MAPPED_SUBRESOURCE ms;
		this->deviceContext->Map(textureData->texture, 0, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

		memcpy(ms.pData, colors, sizeof(unsigned int) * size * size);
		this->deviceContext->Unmap(textureData->texture, 0);
	}

	

	FractalTreeRenderer::~FractalTreeRenderer()
	{
		if(this->perlinBuffer != NULL)
			this->perlinBuffer->Release();

		for(unsigned int a = 0; a < FractalTreeResources::StaticModels.size(); ++a)
		{
			delete FractalTreeResources::StaticModels[a];
		}
	}
}