#include "pch.h"
#include "SpaceGameRenderer.h"

using namespace std;

namespace SpaceGameCore
{
	void SpaceGameRenderer::initializeSimpleVertexShader()
	{
		HRESULT hr;
		std::ifstream myFile(this->shaderDir + "SimpleVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if (myFile.is_open())
		{
			const size_t fileSize = myFile.tellg();
			myFile.seekg(0, std::ios::beg);
			char* shaderData = new char[fileSize];
			myFile.read(shaderData, fileSize);
			myFile.close();
			hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->vertexShaders[SimpleVertexShader]);
			if (FAILED(hr)) { throw FileLoadException(); }

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

			delete[] shaderData;

			if (FAILED(hr)) { throw FileLoadException(); }
		}
		else
		{
			throw FileLoadException();
		}
	}

	void SpaceGameRenderer::initializeRTSTerrainVertexShader()
	{
		HRESULT hr;
		std::ifstream myFile(this->shaderDir + "RTSTerrainVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if (myFile.is_open())
		{
			const size_t fileSize = myFile.tellg();
			myFile.seekg(0, std::ios::beg);
			char* shaderData = new char[fileSize];
			myFile.read(shaderData, fileSize);
			myFile.close();
			hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->vertexShaders[TerrainVertexShader]);
			if (FAILED(hr)) { throw FileLoadException(); }

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

			if (FAILED(hr)) { throw FileLoadException(); }
		}
		else
		{
			throw FileLoadException();
		}
	}
	void SpaceGameRenderer::initializeFontVertexShader()
	{
		std::ifstream myFile(this->shaderDir + "UIVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if (myFile.is_open())
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

			if (FAILED(hr)) { throw new FileLoadException(); }

		}
		else
		{
			throw FileLoadException();
		}
	}

	void SpaceGameRenderer::initializeWorldFontVertexShader()
	{
		std::ifstream myFile(this->shaderDir + "WorldFontVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if (myFile.is_open())
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

			if (FAILED(hr)) { throw new FileLoadException(); }

		}
		else
		{
			throw FileLoadException();
		}
	}

	void SpaceGameRenderer::initializeSimplePixelShader()
	{
		size_t pixelShaderSize;

		char* pixelShaderData = GetFileDataAndSize2(string(this->shaderDir + "SimplePixelShader.cso").c_str(), pixelShaderSize);

		HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[SimplePixelShader]);
		delete[] pixelShaderData;
		if (FAILED(hr))
		{
			throw new FileLoadException();
		}
	}

	void SpaceGameRenderer::initializeRTSTerrainPixelShader()
	{
		size_t pixelShaderSize;

		char* pixelShaderData = GetFileDataAndSize2(string(this->shaderDir + "RTSTerrainPixelShader.cso").c_str(), pixelShaderSize);

		HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[SimpleMountainPixelShader]);
		delete[] pixelShaderData;
		if (FAILED(hr))
		{
			throw new FileLoadException();
		}
	}

	void SpaceGameRenderer::initializeUIPixelShader()
	{
		std::ifstream myFile(this->shaderDir + "UIPixelShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

		if (myFile.is_open())
		{
			const size_t fileSize = myFile.tellg();
			myFile.seekg(0, std::ios::beg);
			char* shaderData = new char[fileSize];
			myFile.read(shaderData, fileSize);
			myFile.close();
			HRESULT hr = this->device->CreatePixelShader(shaderData, fileSize, nullptr, &this->pixelShaders[UIPixelShader]);
			delete[] shaderData;

			if (FAILED(hr)) { throw new FileLoadException(); }
		}
	}

	void SpaceGameRenderer::initializeFontPixelShader()
	{
		size_t pixelShaderSize;

		const string shaderDir = this->shaderDir + string("FontPixelShader.cso");
		char* pixelShaderData = GetFileDataAndSize2(shaderDir.c_str(), pixelShaderSize);

		HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[FontPixelShader]);
		delete[] pixelShaderData;
		if (FAILED(hr)) { throw new FileLoadException(); }
	}
}