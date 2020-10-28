#include "pch.h"
#include "FractalTreeRenderer.h"
#include <sstream>
#include <fstream>
#include "BasicReaderWriter.h"
#include "..\Common Source\Util\File Parser\ASCII\ParseHelper.h"

using namespace DirectX;

namespace RenderingEngine
{
	void FractalTreeRenderer::initializeSimpleVertexShader()
	{

		HRESULT hr;
		std::ifstream myFile("../x64/Shaders/SimpleVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if(myFile.is_open())
		{
			const size_t fileSize = myFile.tellg();
			myFile.seekg(0, std::ios::beg);
			char* shaderData = new char[fileSize];
			myFile.read(shaderData, fileSize);
			myFile.close();
			hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->vertexShaders[SimpleVertexShader]);
			if(FAILED(hr)) {exit(hr);}

			const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 96, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 112, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 128, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NIL", 0, DXGI_FORMAT_R32_FLOAT, 0, 136, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			hr = this->device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, fileSize, &this->inputLayouts[DefaultInputLayout]);

			//delete[] shaderData;

			if(FAILED(hr)) {exit(hr);}
		}
	}

	void FractalTreeRenderer::initializeSkyVertexShader()
	{
		HRESULT hr;
		std::ifstream myFile("../x64/Shaders/SkyVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if(myFile.is_open())
		{
			const size_t fileSize = myFile.tellg();
			myFile.seekg(0, std::ios::beg);
			char* shaderData = new char[fileSize];
			myFile.read(shaderData, fileSize);
			myFile.close();
			hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->vertexShaders[SkyVertexShader]);
			if(FAILED(hr)) {exit(hr);}

			const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 96, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 112, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 128, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NIL", 0, DXGI_FORMAT_R32_FLOAT, 0, 136, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			hr = this->device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, fileSize, &this->inputLayouts[SkyInputLayout]);

			//delete[] shaderData;

			if(FAILED(hr)) {exit(hr);}
		}
	}

	void FractalTreeRenderer::initializeTerrainVertexShader()
	{
		HRESULT hr;
		std::ifstream myFile("../x64/Shaders/TerrainVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if(myFile.is_open())
		{
			const size_t fileSize = myFile.tellg();
			myFile.seekg(0, std::ios::beg);
			char* shaderData = new char[fileSize];
			myFile.read(shaderData, fileSize);
			myFile.close();
			hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->vertexShaders[TerrainVertexShader]);

			const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			hr = this->device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, fileSize, &this->inputLayouts[TerrainInputLayout]);

			delete[] shaderData;

			if(FAILED(hr)) {FailExit();}
		}
	}

	void FractalTreeRenderer::initializeWireFrameVertexShader()
	{
		HRESULT hr;
		std::ifstream myFile("../x64/Shaders/WireFrameVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if(myFile.is_open())
		{
			const size_t fileSize = myFile.tellg();
			myFile.seekg(0, std::ios::beg);
			char* shaderData = new char[fileSize];
			myFile.read(shaderData, fileSize);
			myFile.close();
			hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->vertexShaders[WireFrameVertexShader]);

			const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			hr = this->device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, fileSize, &this->inputLayouts[WireFrameInputLayout]);

			delete[] shaderData;

			if(FAILED(hr)) {FailExit();}
		}
	}

	void FractalTreeRenderer::initializeFontVertexShader()
	{
		std::ifstream myFile("../x64/Shaders/UIVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if(myFile.is_open())
		{
			const size_t fileSize = myFile.tellg();
			myFile.seekg(0, std::ios::beg);
			char* shaderData = new char[fileSize];
			myFile.read(shaderData, fileSize);
			myFile.close();
			HRESULT hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->vertexShaders[UIVertexShader]);
			const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			hr = this->device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, fileSize, &this->inputLayouts[UIInputLayout]);
			delete[] shaderData;

			if(FAILED(hr)) {FailExit();}

		}
	}

	void FractalTreeRenderer::initializeWorldFontVertexShader()
	{
		std::ifstream myFile("../x64/Shaders/WorldFontVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if(myFile.is_open())
		{
			const size_t fileSize = myFile.tellg();
			myFile.seekg(0, std::ios::beg);
			char* shaderData = new char[fileSize];
			myFile.read(shaderData, fileSize);
			myFile.close();
			HRESULT hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->vertexShaders[WorldFontVertexShader]);
			const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			hr = this->device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, fileSize, &this->inputLayouts[WorldFontInputLayout]);
			delete[] shaderData;

			if(FAILED(hr)) {FailExit();}

		}
	}

	void FractalTreeRenderer::initializeParticleVertexShader()
	{
		std::ifstream myFile("../x64/Shaders/ParticleVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if(myFile.is_open())
		{
			const size_t fileSize = myFile.tellg();
			myFile.seekg(0, std::ios::beg);
			char* shaderData = new char[fileSize];
			myFile.read(shaderData, fileSize);
			myFile.close();
			HRESULT hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->vertexShaders[ParticleVertexShader]);

			if(FAILED(hr)) {exit(1);}
			const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 96, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 112, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 128, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NIL", 0, DXGI_FORMAT_R32_FLOAT, 0, 136, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NILC", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 140, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			hr = this->device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, fileSize, &this->inputLayouts[ParticleInputLayout]);

			delete[] shaderData;

			if(FAILED(hr)) {FailExit();}
		}
	}

	void FractalTreeRenderer::initializeInstancedFoliageVertexShader()
	{

		{
			{
				HRESULT hr;
				std::ifstream myFile("../x64/Shaders/ShadowVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

				if(myFile.is_open())
				{
					const size_t fileSize = myFile.tellg();
					myFile.seekg(0, std::ios::beg);
					char* shaderData = new char[fileSize];
					myFile.read(shaderData, fileSize);
					myFile.close();
					hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->shadowVertexShader);
					if (FAILED(hr)) { delete[] shaderData; FailExit(); }

					const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
					{
						{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					};
					hr = this->device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, fileSize, &this->shadowInputLayout);

					delete[] shaderData;

					if(FAILED(hr)) {FailExit();}
				}
			}

			{
				size_t pixelShaderSize;

				char* pixelShaderData = GetFileDataAndSize2("../x64/Shaders/ShadowPixelShader.cso", pixelShaderSize);

				HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->shadowPixelShader);
				delete[] pixelShaderData;
				if(FAILED(hr))
				{
					FailExit();
				}
			}
		}
		HRESULT hr;
		std::ifstream myFile("../x64/Shaders/InstancedFoliageVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if(myFile.is_open())
		{
			const size_t fileSize = myFile.tellg();
			myFile.seekg(0, std::ios::beg);
			char* shaderData = new char[fileSize];
			myFile.read(shaderData, fileSize);
			myFile.close();
			hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->vertexShaders[InstancedFoliageVertexShader]);
			if (FAILED(hr)) {
				delete[] shaderData; FailExit();
			}

			const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "mTransform", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "mTransform", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "mTransform", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "mTransform", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			};
			hr = this->device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, fileSize, &this->inputLayouts[InstancedInputLayout]);

			delete[] shaderData;

			if(FAILED(hr)) {FailExit();}
		}
	}

	void FractalTreeRenderer::initializeSimplePixelShader()
	{
		size_t pixelShaderSize;

		char* pixelShaderData = GetFileDataAndSize2("../x64/Shaders/SimplePixelShader.cso", pixelShaderSize);

		HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[SimplePixelShader]);
		delete[] pixelShaderData;
		if(FAILED(hr))
		{
			FailExit();
		}
	}

	void FractalTreeRenderer::initializeSkyPixelShader()
	{
		size_t pixelShaderSize;

		char* pixelShaderData = GetFileDataAndSize2("../x64/Shaders/SkyPixelShader.cso", pixelShaderSize);

		HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[SkyPixelShader]);
		delete[] pixelShaderData;
		if(FAILED(hr))
		{
			FailExit();
		}
	}

	void FractalTreeRenderer::initializeWaterPixelShader()
	{
		size_t pixelShaderSize;

		const char* const pixelShaderData = GetFileDataAndSize2("../x64/Shaders/WaterPixelShader.cso", pixelShaderSize);

		const HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[WaterPixelShader]);
		delete[] pixelShaderData;
		if (FAILED(hr))
		{
			FailExit();
		}
	}

	void FractalTreeRenderer::initializeWireFramePixelShader()
	{
		size_t pixelShaderSize;

		char* pixelShaderData = GetFileDataAndSize2("../x64/Shaders/WireFramePixelShader.cso", pixelShaderSize);

		HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[WireFramePixelShader]);
		delete[] pixelShaderData;
		if(FAILED(hr))
		{
			FailExit();
		}
	}

	void FractalTreeRenderer::initializeSimpleMountainPixelShader()
	{
		size_t pixelShaderSize;

		char* pixelShaderData = GetFileDataAndSize2("../x64/Shaders/SimpleMountainPixelShader.cso", pixelShaderSize);

		HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[SimpleMountainPixelShader]);
		delete[] pixelShaderData;
		if(FAILED(hr)){FailExit();}
	}

	void FractalTreeRenderer::initializeUIPixelShader()
	{
		std::ifstream myFile("../x64/Shaders/UIPixelShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if(myFile.is_open())
		{
			const size_t fileSize = myFile.tellg();
			myFile.seekg(0, std::ios::beg);
			char* shaderData = new char[fileSize];
			myFile.read(shaderData, fileSize);
			myFile.close();
			HRESULT hr = this->device->CreatePixelShader(shaderData, fileSize, nullptr, &this->pixelShaders[UIPixelShader]);
			delete[] shaderData;

			if(FAILED(hr)){FailExit();}
		}
	}

	void FractalTreeRenderer::initializeFontPixelShader()
	{
		size_t pixelShaderSize;

		char* pixelShaderData = GetFileDataAndSize2("../x64/Shaders/FontPixelShader.cso", pixelShaderSize);

		HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[FontPixelShader]);
		delete[] pixelShaderData;
		if(FAILED(hr)){FailExit();}
	}

	void FractalTreeRenderer::initializeTreePixelShader()
	{
		size_t pixelShaderSize;

		char* pixelShaderData = GetFileDataAndSize2("../x64/Shaders/TreePixelShader.cso", pixelShaderSize);

		HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[TreePixelShader]);
		delete[] pixelShaderData;
		if(FAILED(hr)){FailExit();}
	}

	void FractalTreeRenderer::initializeParticlePixelShader()
	{
		size_t pixelShaderSize;

		char* pixelShaderData = GetFileDataAndSize2("../x64/Shaders/ParticlePixelShader.cso", pixelShaderSize);

		HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[ParticlePixelShader]);
		delete[] pixelShaderData;
		if(FAILED(hr)){FailExit();}
	}

	void FractalTreeRenderer::initializeBloomCombineShader()
	{
		size_t pixelShaderSize;

		char* pixelShaderData = GetFileDataAndSize2("../x64/Shaders/BloomCombineShader.cso", pixelShaderSize);

		HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[BloomCombineShader]);
		delete[] pixelShaderData;
		if(FAILED(hr)){FailExit();}

		this->bloomCombinePixelShader = this->pixelShaders[BloomCombineShader];
	}

	void FractalTreeRenderer::initializeBlurShader()
	{
		size_t pixelShaderSize;

		char* pixelShaderData = GetFileDataAndSize2("../x64/Shaders/BlurShader.cso", pixelShaderSize);

		HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[BlurShader]);
		delete[] pixelShaderData;
		if(FAILED(hr)){FailExit();}

		this->blurPixelShader = this->pixelShaders[BlurShader];
	}

	void FractalTreeRenderer::initializeFoliagePixelShader()
	{
		size_t pixelShaderSize;

		char* pixelShaderData = GetFileDataAndSize2("../x64/Shaders/FoliagePixelShader.cso", pixelShaderSize);

		HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[FoliagePixelShader]);
		delete[] pixelShaderData;
		if(FAILED(hr)){FailExit();}
	}
}