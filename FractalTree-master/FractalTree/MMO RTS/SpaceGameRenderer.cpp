#include "pch.h"
#include "SpaceGameRenderer.h"
#include "..\SpaceGame Common\Core\SpaceGameResources.h"
#include "..\Common Source\Util\Texture Loader\DDSTextureLoader.h"

namespace SpaceGameCore
{
	void SpaceGameRenderer::InitializeOther()
	{
		__super::InitializeOther();
		SpaceGameResources::LoadResources(this);
		this->initializeUI(FontSamplerState, UIPixelShader, UIVertexShader, UIInputLayout, FontTexture, FontPixelShader, FontResourceView);

	}

	void SpaceGameRenderer::InitializeShaders()
	{
#ifdef _DEBUG
		this->shaderDir = "../x64/Debug/Shaders/";
#else
		this->shaderDir = "../x64/Release/Shaders/";
#endif
		//this->shaderDir = "Shaders/";
		this->vertexShaders.resize(8);
		this->pixelShaders.resize(11);
		this->inputLayouts.resize(8);

		this->initializeSimpleVertexShader();
		this->initializeFontVertexShader();
		this->initializeWorldFontVertexShader();
		this->initializeSimplePixelShader();
		this->initializeUIPixelShader();
		this->initializeFontPixelShader();
		this->initializeRTSTerrainVertexShader();
		this->initializeRTSTerrainPixelShader();
	}

	void SpaceGameRenderer::InitializeTextures()
	{
		this->samplerStates.resize(5);
		this->textures.resize(12);
		this->shaderResourceViews.resize(11);
		this->blendStates.resize(2);
		HRESULT hr;
		/*hr = CreateDDSTextureFromFile(this->device,
			L"Resources/Textures/dirt2.dds",
			&this->textures[MountainTexture],
			&this->shaderResourceViews[MountainResourceView]);
		if (FAILED(hr))
		{
			throw FileLoadException();
		}*/
		hr = CreateDDSTextureFromFile(this->device,
			L"Resources/Textures/grass2.dds",
			&this->textures[GrassTexture],
			&this->shaderResourceViews[GrassResourceView]);
		/*if (FAILED(hr))
		{
			throw FileLoadException();
		}
		hr = CreateDDSTextureFromFile(this->device,
			L"Resources/Textures/Stone_DarkGray.dds",
			&this->textures[SlabTexture],
			&this->shaderResourceViews[SlabResourceView]);
		if (FAILED(hr))
		{
			throw FileLoadException();
		}
		hr = CreateDDSTextureFromFile(this->device,
			L"Resources/Textures/stone_1_2048x2048.dds",
			&this->textures[StoneTexture],
			&this->shaderResourceViews[StoneResourceView]);
		if (FAILED(hr))
		{
			throw FileLoadException();
		}
		hr = CreateDDSTextureFromFile(this->device,
			L"Resources/Textures/displacement.dds",
			&this->textures[BumpMapTexture],
			&this->shaderResourceViews[BumpmapResourceView]);
		if (FAILED(hr))
		{
			throw FileLoadException();
		}
		hr = CreateDDSTextureFromFile(this->device,
			L"Resources/Textures/SnowIceTexture0003.dds",
			&this->textures[IceTexture],
			&this->shaderResourceViews[IceResourceView]);
		if (FAILED(hr))
		{
			throw FileLoadException();
		}
		hr = CreateDDSTextureFromFile(this->device,
			L"Resources/Textures/corgifullb.dds",
			&this->textures[ExplosionTexture],
			&this->shaderResourceViews[ExplosionResourceView]);
		if (FAILED(hr))
		{
			throw FileLoadException();
		}
		hr = CreateDDSTextureFromFile(this->device,
			L"Resources/Textures/grasswip1.dds",
			&this->textures[LeafTexture1],
			&this->shaderResourceViews[LeafTexture1ResourceView]);
		if (FAILED(hr))
		{
			throw FileLoadException();
		}
		hr = CreateDDSTextureFromFile(this->device,
			L"Resources/Textures/Leaf_1_Normal.dds",
			&this->textures[LeafTextureNormal1],
			&this->shaderResourceViews[LeafTextureNormal1ResourceView]);
		if (FAILED(hr))
		{
			throw FileLoadException();
		}
		hr = CreateDDSTextureFromFile(this->device,
			L"Resources/Textures/organtexture.dds",
			&this->textures[OrganTexture],
			&this->shaderResourceViews[OrganTextureResourceView]);
		if (FAILED(hr))
		{
			throw FileLoadException();
		}*/
		this->deviceContext->PSSetShaderResources(2, 1, &this->shaderResourceViews[GrassResourceView]);
		this->deviceContext->PSSetShaderResources(4, 1, &this->shaderResourceViews[SlabResourceView]);
		this->deviceContext->PSSetShaderResources(5, 1, &this->shaderResourceViews[StoneResourceView]);
		this->deviceContext->PSSetShaderResources(6, 1, &this->shaderResourceViews[BumpmapResourceView]);
		this->deviceContext->PSSetShaderResources(7, 1, &this->shaderResourceViews[IceResourceView]);
		this->deviceContext->PSSetShaderResources(8, 1, &this->shaderResourceViews[ExplosionResourceView]);
		this->deviceContext->PSSetShaderResources(9, 1, &this->shaderResourceViews[LeafTextureNormal1ResourceView]);

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
		if (hr = FAILED(this->device->CreateSamplerState(&samplerDesc2, &this->samplerStates[TerrainSamplerState])))
		{
			throw int(23);
		}

	}
}
