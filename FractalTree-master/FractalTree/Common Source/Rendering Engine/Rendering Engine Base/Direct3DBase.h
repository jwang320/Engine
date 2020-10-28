#pragma once
#ifndef _NO_GRAPHICS
#include <vector>
#ifdef _WIN32_APPLICATION
#include <d3d11.h>
typedef HWND SystemWindowType;
#else
#include <D3D11_1.h>
typedef int SystemWindowType;

#endif
typedef unsigned int shaderindextype;
#define DEFAULT_INDEX_FORMAT DXGI_FORMAT_R32_UINT

namespace RenderingEngine
{
	struct FontCharacterDefinition
	{
		char character;
		float textureCoordinates[2];
		int pixelWidth;
	};

	struct FontDefinition
	{
		FontCharacterDefinition characterDefinitions[256];
		int characterHeight;
		float characterWidth;
		int characterSpacing;
	};

	//This class is used to access the device context and device from anywhere
	class RenderDevices
	{
	public:
#ifdef _WIN32_APPLICATION
		static ID3D11DeviceContext* deviceContext;
		static ID3D11Device* device;
#else
		static ID3D11DeviceContext1* deviceContext;
		static ID3D11Device1* device;
#endif
	};

	//This class handles the basic interaction with Direct3D.  Actual rendering is deferred to child classes.
	class Direct3DBase
	{
	protected:
		SystemWindowType systemWindow;
#ifdef _WIN32_APPLICATION
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* deviceContext = nullptr;
		IDXGISwapChain* swapChain = nullptr;
#else
		ID3D11Device1* device;
		ID3D11DeviceContext1* deviceContext;
		IDXGISwapChain1* swapChain;
#endif
		ID3D11RenderTargetView* renderTargetView = nullptr;
		ID3D11DepthStencilView* depthStencilView = nullptr;
		//These vectors are manually managed.
		std::vector<ID3D11VertexShader*> vertexShaders;
		std::vector<ID3D11PixelShader*> pixelShaders;
		std::vector<ID3D11InputLayout*> inputLayouts;
		std::vector<ID3D11RasterizerState*> rasterizerStates;
		std::vector<ID3D11Resource*> textures;
		std::vector<ID3D11SamplerState*> samplerStates;
		std::vector<ID3D11ShaderResourceView*> shaderResourceViews;
		std::vector<ID3D11DepthStencilState*> depthStencilStates;
		std::vector<ID3D11BlendState*> blendStates;
		D3D_FEATURE_LEVEL featureLevel;

		int width;
		int height;
		bool fullScreen;
		int sampleCount = 8;
		int sampleQuality = 0;

		D3D11_VIEWPORT windowViewPort;

	public:
		Direct3DBase();
		void Initialize(SystemWindowType systemWindow, bool fullScreen = true, int width = 1600, int height = 900);
		void Present();
		int GetWidth() const {return this->width;}
		int GetHeight() const {return this->height;}
		ID3D11VertexShader* GetVertexShader(int index) const {return this->vertexShaders[index];}
		ID3D11PixelShader* GetPixelShader(int index) const {return this->pixelShaders[index];}
		ID3D11InputLayout* GetInputLayout(int index) const {return this->inputLayouts[index];}
		ID3D11SamplerState* GetSamplerState(int index) const {return this->samplerStates[index];}
		ID3D11ShaderResourceView* GetShaderResourceView(int index) const {return this->shaderResourceViews[index];}
		void Test();// { this->Initialize(0); }
		virtual ~Direct3DBase();

	protected:
		//Override to add subclass-specific initialization. It is called after
		//all default initialization functions.
		virtual void InitializeOther() = 0;

		//Must be overriden to initialize all shaders for the program.
		virtual void InitializeShaders() = 0;

		//Must be override to initalize all textures for the program.
		virtual void InitializeTextures() = 0;

		//Must be overriden to initialize constant buffers for the program.
		virtual void InitializeBuffers() = 0;

		//Must be overriden to initialize rasterizer states for the program
		virtual void InitializeRasterizerStates() = 0;

	private:
#ifdef _WIN32_APPLICATION
		void InitializeDevice(SystemWindowType systemWindow, bool fullScreen, int width, int height);
#else
	public:
		//virtual void InitializeDevice(int width, int height) = 0;
	private:
#endif
		void InitializeView();
		void InitializeDepthBuffer();
		void InitializeViewport();

	protected:
		std::vector<IDXGIAdapter1*> EnumerateAdapters(void);

	};
}
#endif