#include "pch.h"

#include "GameRenderer.h"
#include "..\Src\UIRenderer.h"
#include "GameResources.h"
#include "..\Common Source\Util\Texture Loader\DDSTextureLoader.h"

using namespace RenderingEngine;
using namespace DirectX;

GameRenderer::GameRenderer()
{
	this->useHDR = false;
}

void GameRenderer::InitializeOther()
{
	__super::InitializeOther();
	GameResources::LoadResources(this);
	this->initializeUI(FontSamplerState, UIPixelShader, UIVertexShader, UIInputLayout, FontTexture, FontPixelShader, FontResourceView);
}

#ifndef _WIN32_APPLICATION
void GameRenderer::InitializeDevice(Platform::Agile<Windows::UI::Core::CoreWindow> window)
{
	this->thisWindow = window;
	HRESULT hr;

	this->width = width;
	this->height = height;
	this->fullScreen = fullScreen;
	std::vector<IDXGIAdapter1*> adapters = EnumerateAdapters();

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	// If the project is in a debug build, enable debugging via SDK Layers with this flag.
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	hr = D3D11CreateDevice(
		nullptr,                    // specify null to use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,                    // leave as nullptr unless software device
		creationFlags,              // optionally set debug and Direct2D compatibility flags
		featureLevels,              // list of feature levels this app can support
		ARRAYSIZE(featureLevels),   // number of entries in above list	
		D3D11_SDK_VERSION,          // always set this to D3D11_SDK_VERSION
		&device,                    // returns the Direct3D device created
		&this->featureLevel,        // returns feature level of device created
		&context                    // returns the device immediate context
		);

	if (FAILED(hr))
	{
		assert(false);
	}

	this->device = reinterpret_cast<ID3D11Device1*>(device);
	this->deviceContext = reinterpret_cast<ID3D11DeviceContext1*>(context);
	IDXGIFactory2* dxgiFactory;
	hr = adapters[0]->GetParent(__uuidof(IDXGIFactory2), (void **) &dxgiFactory);
	if (FAILED(hr))
	{
		assert(false);
	}
	IDXGIOutput* outp;
	if (adapters[0]->EnumOutputs(0, &outp) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC outdesc;
		outp->GetDesc(&outdesc);
		this->width = outdesc.DesktopCoordinates.right;
		this->height = outdesc.DesktopCoordinates.bottom;
	}
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.Width = this->width;                                     // use automatic sizing
	swapChainDesc.Height = this->height;
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;                    // use two buffers to enable flip effect
	//swapChainDesc.BufferCount = 1;
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.Flags = 0;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;


	Windows::UI::Core::CoreWindow^ p = window.Get();

	// Create a swap chain for this window from the DXGI factory.

	hr = dxgiFactory->CreateSwapChainForCoreWindow(
		this->device,
		reinterpret_cast<IUnknown*>(p),
		&swapChainDesc,
		nullptr,    // allow on all displays
		&this->swapChain
		);
	if (FAILED(hr))
	{
		assert(false);
	}
	// Ensure that DXGI does not queue more than one frame at a time. This both reduces 
	// latency and ensures that the application will only render after each VSync, minimizing 
	// power consumption.
	//dxgiDevice->SetMaximumFrameLatency(1);
	UINT tt[35];
	for (unsigned int a = 0; a < 32; ++a)
	{
		this->device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, a + 1, tt + a + 1);
	}

	for (unsigned int a = 0; a < adapters.size(); ++a)
	{
		adapters[a]->Release();
	}

	//hr = this->swapChain->ResizeBuffers(2, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
}
#endif
#ifdef _WIN32_APPLICATION
void GameRenderer::InitializeDevice2(HWND systemWindow, bool fullScreen, int width, int height)
{
	HRESULT hr;

	this->thisWindow = systemWindow;
	this->width = width;
	this->height = height;
	this->fullScreen = fullScreen;
	std::vector<IDXGIAdapter1*> adapters = EnumerateAdapters();

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	// If the project is in a debug build, enable debugging via SDK Layers with this flag.
	//creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#else
	this->sampleCount = 8;
#endif
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = this->width;
	sd.BufferDesc.Height = this->height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//sd.BufferDesc.RefreshRate.Numerator = 60;
	//sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = this->thisWindow;
	sd.SampleDesc.Count = this->sampleCount;
	sd.SampleDesc.Quality = this->sampleQuality;
	sd.Windowed = true;//!fullScreen;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	const D3D_FEATURE_LEVEL  FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
	const UINT               numFeatureLevelsRequested = 7;

	hr = D3D11CreateDeviceAndSwapChain(adapters[0],
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		creationFlags,
		featureLevels,
		numFeatureLevelsRequested,
		D3D11_SDK_VERSION,
		&sd,
		&this->swapChain,
		&this->device,
		&this->featureLevel,
		&this->deviceContext);

	if (FAILED(hr))
	{
		exit(hr);
		FailExit();
	}

	UINT tt[35];
	for (unsigned int a = 0; a < 32; ++a)
	{
		this->device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, a + 1, tt + a + 1);
	}

	for (unsigned int a = 0; a < adapters.size(); ++a)
	{
		adapters[a]->Release();
	}

	//hr = this->swapChain->ResizeBuffers(2, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
}
#endif

void GameRenderer::InitializeShaders()
{
	this->vertexShaders.resize(8);
	this->pixelShaders.resize(11);
	this->inputLayouts.resize(8);

	this->initializeSimpleVertexShader();
	this->initializeSimpleInstancedVertexShader();
	this->initializeFontVertexShader();
	this->initializeSimplePixelShader();
	this->initializeUIPixelShader();
	this->initializeFontPixelShader();
}

void GameRenderer::InitializeTextures()
{
	this->samplerStates.resize(5);
	this->textures.resize(12);
	this->shaderResourceViews.resize(11);
	this->blendStates.resize(2);


	HRESULT hr = CreateDDSTextureFromFile(this->device,
		L"../Resources/Textures/displacement.dds",
		&this->textures[BumpMapTexture],
		&this->shaderResourceViews[BumpmapResourceView]);
	if (FAILED(hr))
	{
		exit(7);
	}
	this->deviceContext->PSSetShaderResources(2, 1, &this->shaderResourceViews[BumpmapResourceView]);
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
		exit(22);
	}

	this->deviceContext->PSSetSamplers(1, 1, &this->samplerStates[TerrainSamplerState]);
}

void GameRenderer::InitializeRasterizerStates()
{
	this->rasterizerStates.resize(2);
	const D3D11_RASTERIZER_DESC rState =
	{
		D3D11_FILL_SOLID,
		D3D11_CULL_BACK,
		true,
		0,
		0.f,
		0.f,
		true,
		false,
		false,
		false
	};

	if (FAILED(this->device->CreateRasterizerState(&rState, &this->rasterizerStates[DefaultRasterizerState])))
	{
		exit(34);
	}
	D3D11_RASTERIZER_DESC rState2 =
	{
		D3D11_FILL_WIREFRAME,
		D3D11_CULL_BACK,
		true,
		0,
		0.f,
		0.f,
		true,
		false,
		false,
		false
	};

	if (FAILED(this->device->CreateRasterizerState(&rState2, &this->rasterizerStates[WireframeRasterizerState])))
	{
		exit(34);
	}

	this->deviceContext->RSSetState(this->rasterizerStates[DefaultRasterizerState]);
}

InstanceBufferData* GameRenderer::CreateInstanceBufferData(unsigned int instanceCount) const
{
	InstanceBufferData* instanceBufferData = new InstanceBufferData();
	instanceBufferData->instanceCount = 0;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(ColoredInstanceVertex) * instanceCount;

	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	if (HRESULT hr = FAILED(device->CreateBuffer(&bufferDesc, NULL, &instanceBufferData->instanceBuffer)))
	{
		assert(false);
		exit(20);
	}

	instanceBufferData->instanceSize = sizeof(ColoredInstanceVertex);
	instanceBufferData->instanceOffset = 0;
	return instanceBufferData;
}

void GameRenderer::UpdateInstanceBufferData(InstanceBufferData* instanceBufferdata, ColoredInstanceVertex* transforms) const
{
	D3D11_MAPPED_SUBRESOURCE ms2;
	deviceContext->Map(instanceBufferdata->instanceBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms2);
	memcpy(ms2.pData, transforms, sizeof(ColoredInstanceVertex) * instanceBufferdata->instanceCount);
	deviceContext->Unmap(instanceBufferdata->instanceBuffer, NULL);
}

GameRenderer::~GameRenderer()
{
/*	for (unsigned int a = 0; a < ResourceManager::StaticModels.size(); ++a)
	{
		delete ResourceManager::StaticModels[a];
	} */
}