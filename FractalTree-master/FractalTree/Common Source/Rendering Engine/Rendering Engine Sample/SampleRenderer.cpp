#include "pch.h"
#include "SampleRenderer.h"
#include <fstream>
#include <sstream>
#ifdef _RTS_GAME
#include "UIRenderer.h"
#else
#include "..\src\UIRenderer.h"
#endif
using namespace std;
#include "..\Common Source\Util\File Parser\ASCII\ParseHelper.h"
#include "..\Common Source\Util\Texture Loader\DDSTextureLoader.h"
#include "..\Common Source\Util\Model Parser\3DS Max\ASCIIModelParser.h"
using namespace DirectX;

namespace RenderingEngine
{

	template Model* SampleRenderer::CreateStandardModel<VertexColor>(int vertexShaderID, int pixelShaderID, int inputLayoutID, int vertexCount, int indexCount) const;
	template Model* SampleRenderer::CreateStandardModel<TexturedVertex>(int vertexShaderID, int pixelShaderID, int inputLayoutID, int vertexCount, int indexCount) const;
	template Model* SampleRenderer::CreateStandardModel<VertexPositionColor>(int vertexShaderID, int pixelShaderID, int inputLayoutID, int vertexCount, int indexCount) const;

	SampleRenderer::SampleRenderer()
	{

	}

	void SampleRenderer::Render()
	{
		__super::Render();
		this->uiRenderer->RenderComposedUITree(this->window);
	}

	bool SampleRenderer::initializeUI(int FontSamplerState, int UIPixelShader, int UIVertexShader, int UIInputLayout, int FontTexture, int FontPixelShader, int FontResourceView)
	{
#ifndef _WIN8_1
		HRESULT hr = CreateDDSTextureFromFile(this->device,
			L"../Resources/Textures/Fonts/font.dds",
			&this->textures[FontTexture],
			&this->shaderResourceViews[FontTexture]);
#else
		HRESULT hr = CreateDDSTextureFromFile(this->device,
			L"Assets/Resources/Textures/Fonts/font.dds",
			&this->textures[FontTexture],
			&this->shaderResourceViews[FontTexture]);
#endif
		if(FAILED(hr))
		{
			hr = CreateDDSTextureFromFile(this->device,
				L"Assets/Resources/Textures/Fonts/font.dds",
				&this->textures[FontTexture],
				&this->shaderResourceViews[FontTexture]);
			if (FAILED(hr)) {
				throw int();
			}
		}
		// Create a texture sampler state description.
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
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
		hr = device->CreateSamplerState(&samplerDesc, &this->samplerStates[FontSamplerState]);
		if(FAILED(hr))
		{
			throw int();
		}

		ModelShaderData uiShaderData(this->vertexShaders[UIVertexShader], this->pixelShaders[UIPixelShader]);
		uiShaderData.inputLayout = this->inputLayouts[UIInputLayout];
		uiShaderData.texture = NULL;
		uiShaderData.textureResourceView = NULL;
		uiShaderData.textureSamplerState = NULL;
		uiShaderData.primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		uiShaderData.alphaMap = NULL;
		ModelShaderData fontShaderData(this->vertexShaders[UIVertexShader], this->pixelShaders[FontPixelShader]);

		fontShaderData.inputLayout = this->inputLayouts[UIInputLayout];
		fontShaderData.primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		fontShaderData.resourceViewSlot = 0;
		fontShaderData.samplerStateSlot = 0;
		fontShaderData.textureResourceView = this->shaderResourceViews[FontResourceView];
		fontShaderData.textureSamplerState = this->samplerStates[FontSamplerState];
		fontShaderData.texture = NULL;
		fontShaderData.alphaMap = NULL;
		this->uiRenderer = new UIRenderer(this->deviceContext, this->device, this->oncePerModelBuffer, this->width, this->height, uiShaderData, fontShaderData, this->featureLevel);
		return true;
	}

	template<class VertexType>
	Model* SampleRenderer::CreateStandardModel(int vertexShaderID, int pixelShaderID, int inputLayoutID, int vertexCount, int indexCount) const
	{
		Model* newModel = new Model(this->CreateStandardBufferData<VertexType>(vertexCount, indexCount), this->CreateStandardModelShaderData(vertexShaderID, pixelShaderID, inputLayoutID));
		return newModel;
	}

	template<class VertexType>
	ModelBufferData* SampleRenderer::CreateStandardBufferData(int vertexCount, int indexCount) const
	{
		int indCount[1] = {indexCount};
		return SampleRenderer::CreateStandardBufferData<VertexType>(vertexCount, indCount, 1);
	}

	template<class VertexType>
	void SampleRenderer::CreateStandardVertexBuffer(ModelBufferData* const bufferData, const int vertexCount) const
	{
		bufferData->maxVertexCount = vertexCount;
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		if (FAILED(device->CreateBuffer(&bufferDesc, NULL, &bufferData->vertexBuffer)))
		{
			assert(false);
			exit(20);
		}
	}

	void SampleRenderer::CreateStandardIndexBuffers(ModelBufferData* const bufferData, const int* indexCount, const int indexBufferCount) const
	{
		bufferData->maxIndexCount = indexCount[0];
		for (int a = 0; a < indexBufferCount; ++a)
		{
			D3D11_BUFFER_DESC indexBufferDesc;

			indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			indexBufferDesc.ByteWidth = sizeof(shaderindextype) * indexCount[a];

			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			indexBufferDesc.MiscFlags = 0;
			if (FAILED(device->CreateBuffer(&indexBufferDesc, NULL, &bufferData->indexBuffer[a])))
			{
				assert(false == "broken");
				exit(20);
			}
		}
	}

	template<class VertexType>
	ModelBufferData* SampleRenderer::CreateStandardBufferData(int vertexCount, int* indexCount, int indexBufferCount) const
	{
		ModelBufferData* const newBufferData = new ModelBufferData(indexBufferCount);

		this->CreateStandardVertexBuffer<VertexType>(newBufferData, vertexCount);
		this->CreateStandardIndexBuffers(newBufferData, indexCount, indexBufferCount);
		

		newBufferData->indexFormat = DEFAULT_INDEX_FORMAT;
		newBufferData->vertexSize = sizeof(VertexType);
		newBufferData->indexOffset = 0;
		newBufferData->vertexOffset = 0;
		for(int a = 0; a < indexBufferCount; ++a)
		{
			newBufferData->indexCount[a] = 0;
		}

		newBufferData->vertexCount = 0;
		return newBufferData;
	}

	ModelBufferData* SampleRenderer::CreateBufferDataFromVertexData(int vertexCount,
		int indexCount,
		XMVECTOR* vertices,
		XMVECTOR* colors,
		XMVECTOR* normals,
		shaderindextype* indices,
		XMVECTOR* tangents,
		XMVECTOR* binormals) const
	{
		ModelBufferData* bufferData = new ModelBufferData();
		bufferData->indexCount[0] = indexCount;
		bufferData->vertexCount = vertexCount;
		bufferData->indexFormat = DEFAULT_INDEX_FORMAT;

		bufferData->vertexSize = sizeof(VertexPositionColor);
		bufferData->indexOffset = 0;
		bufferData->vertexOffset = 0;

		if(indexCount != 0)
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = sizeof(VertexPositionColor) * bufferData->vertexCount;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;

			VertexPositionColor* newVertices = new VertexPositionColor[vertexCount];
			for(int a = 0; a < vertexCount; ++a)
			{
				newVertices[a].color = colors[a];
				newVertices[a].normal = normals[a];
				newVertices[a].pos = vertices[a];
				XMFLOAT4 p, n;
				XMStoreFloat4(&p, newVertices[a].pos);
				XMStoreFloat4(&n, newVertices[a].normal);
				if(tangents != NULL)
				{
					newVertices[a].tangent = tangents[a];
					newVertices[a].binormal = binormals[a];
				}
				else if (n.z > 0)
				{
					newVertices[a].tangent = XMVectorSet(0, 1, 0, 0);
					newVertices[a].binormal = XMVectorSet(1, 0, 0, 0);
				}
				else if (n.y > 0)
				{
					newVertices[a].tangent = XMVectorSet(0, 0, 1, 0);
					newVertices[a].binormal = XMVectorSet(-1, 0, 0, 0);
				}
				else if (n.y < 0)
				{
					newVertices[a].tangent = XMVectorSet(0, 0, 1, 0);
					newVertices[a].binormal = XMVectorSet(1, 0, 0, 0);
				}
				else if (n.x < 0)
				{
					newVertices[a].tangent = XMVectorSet(0, 0, 1, 0);
					newVertices[a].binormal = XMVectorSet(0, 1, 0, 0);
				}
				else if (n.x > 0)
				{
					newVertices[a].tangent = XMVectorSet(0, 1, 0, 0);
					newVertices[a].binormal = XMVectorSet(0, 0,1, 0);
				}

				newVertices[a].emissiveColor = XMVectorSet(0, 0, 0, 0);
				newVertices[a].specularColor = XMVectorSet(1, 1, 1, 1);
				newVertices[a].ambientColor = XMVectorMultiply(newVertices[a].color, XMVectorSet(.05f, .05f, .05f, 1));
				if (n.z > 0)
				{
					newVertices[a].texCoord = XMFLOAT2(p.x < 0 ? 0 : 1.f, p.y < 0 ? 0 : 1.f);
				}
				else if (n.z < 0)
				{
					newVertices[a].texCoord = XMFLOAT2(p.x < 0 ? 0 : 1.f, p.y < 0 ? 0 : 1.f);
				}
				else if (n.y > 0)
				{
					newVertices[a].texCoord = XMFLOAT2(p.x < 0 ? 0 : 1.f, p.z < 0 ? 1.f : 0);
				}
				else if (n.y < 0)
				{
					newVertices[a].texCoord = XMFLOAT2(p.x < 0 ? 0 : 1.f, p.z < 0 ? 1.f : 0);
				}
				else if (n.x < 0)
				{
					newVertices[a].texCoord = XMFLOAT2(p.y < 0 ? 0 : 1.f, p.z < 0 ? 1.f : 0);
				}
				else if (n.x > 0)
				{
					newVertices[a].texCoord = XMFLOAT2(p.y < 0 ? 0 : 1.f, p.z > 0 ? 1.f : 0);
				}
				else
				{
					newVertices[a].texCoord = XMFLOAT2(0, 0);
				}
				newVertices[a].specularExponent = 30;
			}

			D3D11_SUBRESOURCE_DATA InitDataV;
			InitDataV.pSysMem = newVertices;
			InitDataV.SysMemPitch = 0;
			InitDataV.SysMemSlicePitch = 0;
			if(FAILED(device->CreateBuffer(&bd, &InitDataV, &bufferData->vertexBuffer)))      // create the buffer
			{
				exit(20);
			}

			delete[] newVertices;

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
			HRESULT hr = device->CreateBuffer(&bufferDesc, &InitData, &bufferData->indexBuffer[0]);
			if(FAILED(hr))
				exit(21);			
		}

		return bufferData;
	}

	void SampleRenderer::InitializeRasterizerStates()
	{
		this->rasterizerStates.resize(2);
		const D3D11_RASTERIZER_DESC rState =
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

		if(FAILED(this->device->CreateRasterizerState(&rState, &this->rasterizerStates[DefaultRasterizerState])))
		{
			throw int();
		}
		D3D11_RASTERIZER_DESC rState2 =
		{
			D3D11_FILL_WIREFRAME,
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

		if(FAILED(this->device->CreateRasterizerState(&rState2, &this->rasterizerStates[WireframeRasterizerState])))
		{
			throw int();
		}

		this->deviceContext->RSSetState(this->rasterizerStates[DefaultRasterizerState]);
	}

	void SampleRenderer::ParseFileData(const char* fileName,
		unsigned int& vertexCount,
		unsigned int& indexCount,
		DirectX::XMVECTOR** vertices,
		DirectX::XMVECTOR** colors,
		DirectX::XMVECTOR** normals,
		shaderindextype** indices,
		bool usePerVertexNormals,
		bool invertNormals) const
	{
		bool foundVertexCount = false;
		bool foundIndexCount = false;
		bool foundAllIndices = false;
		bool foundAllVertices = false;
		int lineNumber = 0;
		unsigned int faceCount = 0;
		std::string line;
		std::stringstream myFile;
		myFile << fileName;

		while(myFile.good())
		{
			if(!foundVertexCount)
			{
				ParseUnsignedInt(myFile, "*MESH_NUMVERTEX ", vertexCount);
				foundVertexCount = true;
				*vertices = new XMVECTOR[vertexCount];
				*normals = new XMVECTOR[vertexCount];
				*colors = new XMVECTOR[vertexCount];
			}
			else if(!foundIndexCount)
			{
				ParseUnsignedInt(myFile, "*MESH_NUMFACES ", faceCount);
				indexCount = faceCount*3;
				foundIndexCount = true;
				*indices = new shaderindextype[indexCount];
			}
			else if(!foundAllVertices)
			{
				ModelParser::ParseVertices(myFile, "*MESH_VERTEX ", vertexCount, *vertices);
				ModelParser::ParseVertexColors(myFile, "", vertexCount, *colors);
				foundAllVertices = true;
			}
			else if(!foundAllIndices)
			{
				std::getline(myFile, line);
				const int pos = static_cast<int>(line.find("*MESH_FACE "));
				if(pos != std::string::npos)
				{
					int ind1, ind2, ind3, whichFace;
					std::string newStr = line.substr(line.find_first_of("0123456789"));
					const std::string wF = newStr.substr(0,newStr.find_first_of(":"));

					std::istringstream intStream(wF);
					intStream >> whichFace;

					newStr = newStr.substr(newStr.find_first_of(":"));
					newStr = newStr.substr(newStr.find_first_of("0123456789"));
					const std::string i1 = newStr.substr(0, newStr.find_first_not_of("0123456789"));
					{
						std::istringstream intStream2(i1);
						intStream2 >> ind1;
					}

					newStr = newStr.substr(newStr.find_first_of(":"));
					newStr = newStr.substr(newStr.find_first_of("0123456789"));
					const std::string i2 = newStr.substr(0, newStr.find_first_not_of("0123456789"));
					{
						std::istringstream intStream2(i2);
						intStream2 >> ind2;
					}


					newStr = newStr.substr(newStr.find_first_of(":"));
					newStr = newStr.substr(newStr.find_first_of("0123456789"));
					const std::string i3 = newStr.substr(0, newStr.find_first_not_of("0123456789"));
					{
						std::istringstream intStream2(i3);
						intStream2 >> ind3;
					}

					if(invertNormals)
					{
						(*(indices))[whichFace*3] = ind3;
						(*(indices))[whichFace*3 + 1] = ind2;
						(*(indices))[whichFace*3 + 2] = ind1;
					}
					else
					{
						(*(indices))[whichFace*3] = ind1;
						(*(indices))[whichFace*3 + 1] = ind2;
						(*(indices))[whichFace*3 + 2] = ind3;
					}
					if(whichFace*3 == indexCount)
					{
						foundAllIndices = true;
					}
				}
			}

			++lineNumber;
		}

		if(usePerVertexNormals)
		{
			ModelParser::CalculatePerVertexNormals(faceCount, *vertices, *indices, vertexCount, *normals);
		}
		else
		{
			XMVECTOR* newVertices;
			XMVECTOR* newNormals;
			XMVECTOR* newColors;
			ModelParser::CalculatePerFaceStructures(*vertices, *normals, *colors, *indices, faceCount, &newVertices, &newNormals, &newColors, vertexCount);
#ifndef _AMD64_
			_aligned_free(*vertices);
			_aligned_free(*normals);
			_aligned_free(*colors);
#else
			delete [] *vertices;
			delete [] *normals;
			delete [] *colors;
#endif
			*vertices = newVertices;
			*normals = newNormals;
			*colors = newColors;
		}

		if(usePerVertexNormals)
		{
			for(unsigned int ind = 0; ind < vertexCount; ++ind)
			{
				(*(normals))[ind] = XMVector3Normalize((*(normals))[ind]);
			}
		}
	}

	ModelBufferData* SampleRenderer::CreateBufferDataFromString(const char* data, bool usePerVertexNormals, bool invertNormals) const
	{
		unsigned int indexCount = 0;
		//unsigned int faceCount = 0;
		unsigned int vertexCount = 0;

		DirectX::XMVECTOR* vertices = NULL;
		DirectX::XMVECTOR* colors = NULL;
		DirectX::XMVECTOR* normals = NULL;
		shaderindextype* indices = NULL;
		ParseFileData(data, vertexCount, indexCount, &vertices, &colors, &normals, &indices, usePerVertexNormals, invertNormals);
		ModelBufferData* bufferData = this->CreateBufferDataFromVertexData(vertexCount, indexCount, vertices, colors, normals, indices);

		delete[] vertices;
		delete[] colors;
		delete[] normals;
		delete[] indices;

		return bufferData;
	}

	ModelBufferData* SampleRenderer::CreateQuadModel() const
	{/*
		TexturedSimpleVertex vertices[9] = {{XMVectorSet(.5f, 0, 0, 1), XMVectorSet(0, 0, 1, 0), XMFLOAT2(1, 1)},
			{XMVectorSet(.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(1, 0)},
			{XMVectorSet(-.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(0, 0)},
		{XMVectorSet(-.5f, 0, 0, 1), XMVectorSet(0, 0, 1, 0), XMFLOAT2(0, 1)},
		{XMVectorSet(.5f, 0, 0, 1), XMVectorSet(0, 0, -1, 0), XMFLOAT2(1, 1)},
		{XMVectorSet(.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(1, 0)},
		{XMVectorSet(-.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(0, 0)},
		{XMVectorSet(-.5f, 0, 0, 1), XMVectorSet(0, 0, -1, 0), XMFLOAT2(0, 1)}};
		//{XMVectorSet(0, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(.5f, 1)}};

		//shaderindextype indices[18] = {0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6};
		shaderindextype indices[12] = {0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6};*/

		TexturedSimpleVertex vertices[8] = {{XMVectorSet(.5f, 0, 0, 1), XMVectorSet(0, 0, 1, 0), XMFLOAT2(1, 1)},
		//{XMVectorSet(.5f, 1, 0, 1), XMVectorSet(0, 0, 1, 0), XMFLOAT2(1, 0)},
		//{XMVectorSet(-.5f, 1, 0, 1), XMVectorSet(0, 0, 1, 0), XMFLOAT2(0, 0)},
		{XMVectorSet(.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(1, 0)},
		{XMVectorSet(-.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(0, 0)},
		{XMVectorSet(-.5f, 0, 0, 1), XMVectorSet(0, 0, 1, 0), XMFLOAT2(0, 1)},
		{XMVectorSet(.5f, 0, 0, 1), XMVectorSet(0, 0, -1, 0), XMFLOAT2(1, 1)},
		//{XMVectorSet(.5f, 1, 0, 1), XMVectorSet(0, 0, -1, 0), XMFLOAT2(1, 0)},
		//{XMVectorSet(-.5f, 1, 0, 1), XMVectorSet(0, 0, -1, 0), XMFLOAT2(0, 0)},
		{XMVectorSet(.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(1, 0)},
		{XMVectorSet(-.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(0, 0)},
		{XMVectorSet(-.5f, 0, 0, 1), XMVectorSet(0, 0, -1, 0), XMFLOAT2(0, 1)}};

		shaderindextype indices[12] = {0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6};

		ModelBufferData* bufferData = new ModelBufferData();
		bufferData->indexCount[0] = 12;
		bufferData->vertexCount = 8;
		bufferData->indexFormat = DEFAULT_INDEX_FORMAT;

		bufferData->vertexSize = sizeof(TexturedSimpleVertex);
		bufferData->indexOffset = 0;
		bufferData->vertexOffset = 0;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(TexturedSimpleVertex) * bufferData->vertexCount;
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
		HRESULT hr = device->CreateBuffer(&bufferDesc, &InitData, &bufferData->indexBuffer[0]);
		if (FAILED(hr))
			throw int();
		
		
		return bufferData;
	}

	ModelBufferData* SampleRenderer::CreateQuadModel2() const
	{/*
	 TexturedSimpleVertex vertices[9] = {{XMVectorSet(.5f, 0, 0, 1), XMVectorSet(0, 0, 1, 0), XMFLOAT2(1, 1)},
	 {XMVectorSet(.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(1, 0)},
	 {XMVectorSet(-.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(0, 0)},
	 {XMVectorSet(-.5f, 0, 0, 1), XMVectorSet(0, 0, 1, 0), XMFLOAT2(0, 1)},
	 {XMVectorSet(.5f, 0, 0, 1), XMVectorSet(0, 0, -1, 0), XMFLOAT2(1, 1)},
	 {XMVectorSet(.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(1, 0)},
	 {XMVectorSet(-.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(0, 0)},
	 {XMVectorSet(-.5f, 0, 0, 1), XMVectorSet(0, 0, -1, 0), XMFLOAT2(0, 1)}};
	 //{XMVectorSet(0, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMFLOAT2(.5f, 1)}};

	 //shaderindextype indices[18] = {0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6};
	 shaderindextype indices[12] = {0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6};*/

		TexturedVertex vertices[8] = { { XMVectorSet(.5f, 0, 0, 1), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 0, 1, 0), XMFLOAT2(1, 1) },
			//{XMVectorSet(.5f, 1, 0, 1), XMVectorSet(0, 0, 1, 0), XMFLOAT2(1, 0)},
			//{XMVectorSet(-.5f, 1, 0, 1), XMVectorSet(0, 0, 1, 0), XMFLOAT2(0, 0)},
		{ XMVectorSet(.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 0, 1, 0), XMFLOAT2(1, 0) },
		{ XMVectorSet(-.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 0, 1, 0), XMFLOAT2(0, 0) },
		{ XMVectorSet(-.5f, 0, 0, 1), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 0, 1, 0), XMFLOAT2(0, 1) },
		{ XMVectorSet(.5f, 0, 0, 1), XMVectorSet(0, 0, -1, 0), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 0, 1, 0), XMFLOAT2(1, 1) },
			//{XMVectorSet(.5f, 1, 0, 1), XMVectorSet(0, 0, -1, 0), XMFLOAT2(1, 0)},
			//{XMVectorSet(-.5f, 1, 0, 1), XMVectorSet(0, 0, -1, 0), XMFLOAT2(0, 0)},
		{ XMVectorSet(.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 0, 1, 0), XMFLOAT2(1, 0) },
		{ XMVectorSet(-.5f, 1, 0, 1), XMVectorSet(0, 1, 0, 0), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 0, 1, 0), XMFLOAT2(0, 0) },
		{ XMVectorSet(-.5f, 0, 0, 1), XMVectorSet(0, 0, -1, 0), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 0, 1, 0), XMFLOAT2(0, 1) } };

		shaderindextype indices[12] = { 0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6 };

		ModelBufferData* bufferData = new ModelBufferData();
		bufferData->indexCount[0] = 12;
		bufferData->vertexCount = 8;
		bufferData->indexFormat = DEFAULT_INDEX_FORMAT;

		bufferData->vertexSize = sizeof(TexturedVertex);
		bufferData->indexOffset = 0;
		bufferData->vertexOffset = 0;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(TexturedVertex) * bufferData->vertexCount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;


		D3D11_SUBRESOURCE_DATA InitDataV;
		InitDataV.pSysMem = vertices;
		InitDataV.SysMemPitch = 0;
		InitDataV.SysMemSlicePitch = 0;
		if (FAILED(device->CreateBuffer(&bd, &InitDataV, &bufferData->vertexBuffer)))      // create the buffer
		{
			throw int();
		}

		// Fill in a buffer description.
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth = sizeof(shaderindextype) * bufferData->indexCount[0];
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		// Define the resource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		// Create the buffer with the device.
		HRESULT hr = device->CreateBuffer(&bufferDesc, &InitData, &bufferData->indexBuffer[0]);
		if (FAILED(hr))
			throw int();


		return bufferData;
	}

	SampleRenderer::~SampleRenderer()
	{
		if(this->uiRenderer != NULL)
			delete this->uiRenderer;
		if(this->defaultFontDefinition != NULL)
			delete this->defaultFontDefinition;
	}
}