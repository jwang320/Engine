#include "pch.h"
#include "Direct3DBase.h"
#include "Direct3DRenderer.h"
#include "BaseRendererResources.h"

namespace RenderingEngine
{
#ifdef _WIN32_APPLICATION
	ID3D11Device* RenderDevices::device = NULL;
	ID3D11DeviceContext* RenderDevices::deviceContext = NULL;
#else
	ID3D11Device1* RenderDevices::device = NULL;
	ID3D11DeviceContext1* RenderDevices::deviceContext = NULL;
#endif

	Direct3DBase::Direct3DBase()
	{
#ifdef _DEBUG
		this->sampleCount = 1;
		this->sampleQuality = 0;
#endif
	}

	void Direct3DBase::Test()
	{
	}

	void Direct3DBase::Initialize(SystemWindowType systemWindow, bool fullScreen, int width, int height)
	{
#ifdef _WIN32_APPLICATION
		if (systemWindow != 0)
			this->InitializeDevice(systemWindow, fullScreen, width, height);
#else

#endif
		this->InitializeView();
		this->InitializeDepthBuffer();
		this->InitializeViewport();
		this->InitializeShaders();
		this->InitializeTextures();
		this->InitializeRasterizerStates();
		this->InitializeBuffers();
		this->InitializeOther();

		Direct3DRenderer::SetCurrentDeviceContext(this->deviceContext);
		Direct3DRenderer::SetCurrentDevice(this->device);

		RenderDevices::deviceContext = this->deviceContext;
		RenderDevices::device = this->device;
	}

#ifndef _WIN8_1
	void Direct3DBase::InitializeDevice(SystemWindowType systemWindow, bool fullScreen, int width, int height)
	{
		HRESULT hr;

		this->systemWindow = systemWindow;
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

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = this->width;
		sd.BufferDesc.Height = this->height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//sd.BufferDesc.RefreshRate.Numerator = 60;
		//sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = this->systemWindow;
		sd.SampleDesc.Count = this->sampleCount;
		sd.SampleDesc.Quality = this->sampleQuality;
		sd.Windowed = true;//!fullScreen;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		const D3D_FEATURE_LEVEL  FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
		const UINT               numFeatureLevelsRequested = 7;
	
//#ifdef WINAPI_FAMILY_PC_APP
#ifndef _WIN8_1
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
#else
		hr = 0;
#endif
		if(FAILED(hr))
		{
			throw int();
		}

		UINT tt[35];
		for(unsigned int a = 0; a < 32; ++a)
		{
 			this->device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, a + 1, tt + a + 1);
		}

		for(unsigned int a = 0; a < adapters.size(); ++a)
		{
			adapters[a]->Release();
		}

		//hr = this->swapChain->ResizeBuffers(2, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
	}
#endif
	void Direct3DBase::InitializeView()
	{
		// Obtain the backbuffer for this window which will be the final 3D rendertarget.
		ID3D11Texture2D* backBuffer = NULL;
		HRESULT hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

		if(FAILED(hr))
		{
			throw int();
		}

		// Create a view interface on the rendertarget to use on bind.
		hr = this->device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);

		if(FAILED(hr))
		{
			throw int();
		}

		backBuffer->Release();
	}

	void Direct3DBase::InitializeDepthBuffer()
	{
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
		depthStencilDesc.Width = this->width;
		depthStencilDesc.Height = this->height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = this->sampleCount;
		depthStencilDesc.SampleDesc.Quality = this->sampleQuality;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;
		// Allocate a 2-D surface as the depth/stencil buffer.
		ID3D11Texture2D* depthStencil;

		HRESULT hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencil);

		if(FAILED(hr))
		{
			throw int();
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));

		descDSV.Format = depthStencilDesc.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		descDSV.Texture2D.MipSlice = 0;
		// Create a DepthStencil view on this surface to use on bind.
		hr = this->device->CreateDepthStencilView(
			depthStencil,
			&descDSV,
			&depthStencilView);

		if(FAILED(hr))
		{
			throw int();
		}

		this->deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		/*deviceContext->oms*/
		depthStencil->Release();
	}

	void Direct3DBase::InitializeViewport()
	{
		ZeroMemory(&this->windowViewPort, sizeof(D3D11_VIEWPORT));
		this->windowViewPort.TopLeftX = 0;
		this->windowViewPort.TopLeftY = 0;
		this->windowViewPort.MinDepth = 0.0f;
		this->windowViewPort.MaxDepth = 1.0f;
		this->windowViewPort.Width = static_cast<float>(this->width);
		this->windowViewPort.Height = static_cast<float>(this->height);

		// Set the current viewport using the descriptor.
		this->deviceContext->RSSetViewports(1, &this->windowViewPort);
	}

	std::vector <IDXGIAdapter1*> Direct3DBase::EnumerateAdapters(void)
	{
		IDXGIAdapter1 * pAdapter; 
		std::vector <IDXGIAdapter1*> vAdapters; 
		IDXGIFactory1* pFactory = NULL; 
		if(FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory)))
		{
			return vAdapters;
		}

		for (UINT i = 0; pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			vAdapters.push_back(pAdapter); 
		}

		if(pFactory)
		{
			pFactory->Release();
		}

		return vAdapters;
	}

	void Direct3DBase::Present()
	{
		// The first argument instructs DXGI to block until VSync, putting the application
		// to sleep until the next VSync. This ensures we don't waste any cycles rendering
		// frames that will never be displayed to the screen.
		HRESULT hr;
		if(BaseRendererResources::FastEverything)
		{
			hr = this->swapChain->Present(0, 0);
		}
		else
		{
			hr = this->swapChain->Present(1, 0);
		}

		// If the device was removed either by a disconnect or a driver upgrade, we 
		// must completely reinitialize the renderer.
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
#ifndef _NO_METRO
		//	Initialize(m_window.Get());
			FailExit();
#endif
		}
	}

	Direct3DBase::~Direct3DBase()
	{
		for(unsigned int a = 0; a < this->vertexShaders.size(); ++a)
		{
			if(this->vertexShaders[a] != NULL)
				this->vertexShaders[a]->Release();
		}

		for(unsigned int a = 0; a < this->pixelShaders.size(); ++a)
		{
			this->pixelShaders[a]->Release();
		}

		for(unsigned int a = 0; a < this->inputLayouts.size(); ++a)
		{
			if(this->inputLayouts[a] != NULL)
			this->inputLayouts[a]->Release();
		}

		for(unsigned int a = 0; a < this->rasterizerStates.size(); ++a)
		{
			this->rasterizerStates[a]->Release();
		}

		for(unsigned int a = 0; a < this->textures.size(); ++a)
		{
			if(this->textures[a] != NULL)
				this->textures[a]->Release();
		}

		for(unsigned int a = 0; a < this->samplerStates.size(); ++a)
		{
			if(this->samplerStates[a] != NULL)
				this->samplerStates[a]->Release();
		}

		for(unsigned int a = 0; a < this->shaderResourceViews.size(); ++a)
		{
			if(this->shaderResourceViews[a] != NULL)
				this->shaderResourceViews[a]->Release();
		}

		for(unsigned int a = 0; a < this->blendStates.size(); ++a)
		{
			if(this->blendStates[a] != NULL)
				this->blendStates[a]->Release();
		}

		for(unsigned int a = 0; a < this->depthStencilStates.size(); ++a)
		{
			if(this->depthStencilStates[a] != NULL)
				this->depthStencilStates[a]->Release();
		}

		if(this->depthStencilView != NULL)
			this->depthStencilView->Release();

		if(this->renderTargetView != NULL)
			this->renderTargetView->Release();

		if(this->deviceContext != NULL)
			this->deviceContext->Release();
		if(this->swapChain != NULL)
			this->swapChain->Release();
		if(this->device != NULL)
			this->device->Release();
	}
}