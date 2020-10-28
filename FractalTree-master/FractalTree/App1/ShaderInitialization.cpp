#include "pch.h"
#include "GameRenderer.h"
#include "..\Common Source\Util\File Parser\ASCII\ParseHelper.h"

void GameRenderer::initializeSimpleVertexShader()
{

	HRESULT hr;
#ifndef _WIN32_APPLICATION
	std::ifstream myFile("SimpleVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);
#else
	std::ifstream myFile("../x64/Shaders/SimpleVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate); 
#endif
	if (myFile.is_open())
	{
		const size_t fileSize = myFile.tellg();
		myFile.seekg(0, std::ios::beg);
		char* shaderData = new char[fileSize];
		myFile.read(shaderData, fileSize);
		myFile.close();
		hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->vertexShaders[SimpleVertexShader]);
		if (FAILED(hr)) { exit(hr); }

		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

		if (FAILED(hr)) { exit(hr); }
	}
}

void GameRenderer::initializeSimpleInstancedVertexShader()
{
	HRESULT hr;
	std::ifstream myFile("../x64/Shaders/InstancedSimpleVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);

	if (myFile.is_open())
	{
		const size_t fileSize = myFile.tellg();
		myFile.seekg(0, std::ios::beg);
		char* shaderData = new char[fileSize];
		myFile.read(shaderData, fileSize);
		myFile.close();
		hr = this->device->CreateVertexShader(shaderData, fileSize, nullptr, &this->vertexShaders[InstancedFoliageVertexShader]);
		if (FAILED(hr)) { delete[] shaderData; }

		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 96, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 112, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 128, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NIL", 0, DXGI_FORMAT_R32_FLOAT, 0, 136, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "mTransform", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "mTransform", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "mTransform", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "mTransform", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "COLOR", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "COLOR", 5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NILB", 0, DXGI_FORMAT_R32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
		};
		hr = this->device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, fileSize, &this->inputLayouts[InstancedInputLayout]);

		delete[] shaderData;

		if (FAILED(hr)) { FailExit(); }
	}
	}
void GameRenderer::initializeFontVertexShader()
{
#ifndef _WIN32_APPLICATION
	std::ifstream myFile("UIVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);
#else
	std::ifstream myFile("../x64/Shaders/UIVertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate);
#endif

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
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		hr = this->device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, fileSize, &this->inputLayouts[UIInputLayout]);
		delete[] shaderData;

		if (FAILED(hr)) { exit(34); }

	}
}

void GameRenderer::initializeSimplePixelShader()
{
	size_t pixelShaderSize;

#ifndef _WIN32_APPLICATION
	char* pixelShaderData = GetFileDataAndSize2("DefaultPixelShader.cso", pixelShaderSize);
#else
	char* pixelShaderData = GetFileDataAndSize2("../x64/Shaders/DefaultPixelShader.cso", pixelShaderSize);
#endif

	HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[SimplePixelShader]);
	delete[] pixelShaderData;
	if (FAILED(hr))
	{
		exit(34);
	}
}

void GameRenderer::initializeUIPixelShader()
{
#ifndef _WIN32_APPLICATION
	std::ifstream myFile("UIPixelShader.cso", std::ios::in | std::ios::binary | std::ios::ate);
#else
	std::ifstream myFile("../x64/Shaders/UIPixelShader.cso", std::ios::in | std::ios::binary | std::ios::ate);
#endif

	if (myFile.is_open())
	{
		const size_t fileSize = myFile.tellg();
		myFile.seekg(0, std::ios::beg);
		char* shaderData = new char[fileSize];
		myFile.read(shaderData, fileSize);
		myFile.close();
		HRESULT hr = this->device->CreatePixelShader(shaderData, fileSize, nullptr, &this->pixelShaders[UIPixelShader]);
		delete[] shaderData;

		if (FAILED(hr)) { exit(34); }
	}
}

void GameRenderer::initializeFontPixelShader()
{
	size_t pixelShaderSize;
#ifndef _WIN32_APPLICATION
	char* pixelShaderData = GetFileDataAndSize2("FontPixelShader.cso", pixelShaderSize);
#else
	char* pixelShaderData = GetFileDataAndSize2("../x64/Shaders/FontPixelShader.cso", pixelShaderSize);
#endif

	HRESULT hr = this->device->CreatePixelShader(pixelShaderData, pixelShaderSize, nullptr, &this->pixelShaders[FontPixelShader]);
	delete[] pixelShaderData;
	if (FAILED(hr)) { exit(34); }
}