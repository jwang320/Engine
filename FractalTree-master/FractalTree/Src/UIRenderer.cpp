#include "pch.h"
#include "UIRenderer.h"
#include <vector>
#include <string>
#ifdef _USE_METRO
#include "BasicReaderWriter.h"
#endif
//#ifdef _WIN32_APPLICATION
#include <fstream>
//#endif
#include <sstream>
#include "ResourceManager.h"
#include "..\Common Source\SimpleUI\Window.h"
#include "..\Common Source\SimpleUI\UIElement.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Base\ModelBufferData.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Base\Direct3DRenderer.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\ModelVertexTypes.h"
#include "UIMapEditor.h"

using namespace std;
using namespace DirectX;
using namespace SimpleUI;

namespace RenderingEngine
{
	FontDefinition* UIRenderer::defaultFontDefinition = NULL;
	vector<DebugString> UIRenderer::DebugStrings;

	UIRenderer::UIRenderer(ID3D11DeviceContext* deviceContext, ID3D11Device* device, ID3D11Buffer* dataBuffer, int width, int height,
		const ModelShaderData& shaderData,
		const ModelShaderData& fontShaderData, D3D_FEATURE_LEVEL m_featureLevel, ID3D11ShaderResourceView* fontTextureView, ID3D11Resource* fontTexture) :
	deviceContext(deviceContext), device(device), oncePerModelBuffer(dataBuffer), width(width), height(height),
		fontTextureView(fontTextureView), fontTexture(fontTexture)
	{
		this->initializeUIModel(shaderData, m_featureLevel);
		this->initializeFont();
		this->createTextBuffer(fontShaderData, m_featureLevel);
	}

	ModelBufferData* UIRenderer::uiModelPublic = nullptr;
	void UIRenderer::RenderComposedUITree(const SimpleUI::Window* window)
	{

		if (window->showMouse)
		{
			this->uiModelBufferData.model = XMMatrixTranspose(XMMatrixMultiply(XMMatrixScalingFromVector(XMVectorSet(.01f, .01f, 0, 1)), XMMatrixTranslationFromVector(XMVectorSet(window->mouseX, window->mouseY, 0, 1))));
			//this->uiModelBufferData.scaleMatrix = XMMatrixScalingFromVector(XMVectorSet(.01, .01, 0, 1));
			this->uiModelBufferData.tint = XMVectorSet(1, 1, 1, 1);
			this->deviceContext->UpdateSubresource(oncePerModelBuffer, 0, NULL, &uiModelBufferData, 0, 0);
			this->uiModel->modelShaderData.textureResourceView = nullptr;// element->GetTexture();
			this->uiModel->modelShaderData.samplerStateSlot = 1;
			this->uiModel->modelShaderData.resourceViewSlot = 1;
			//this->uiModel->modelShaderData.textureSamplerState = 
			Direct3DRenderer::RenderModel(this->uiModel);
		}
		UIRenderer::renderComposedUITreeHelper(window->GetForegroundUIContainer(), XMMatrixIdentity());
		int skipCount = 0;
		for(unsigned int a = 0; a < UIRenderer::DebugStrings.size(); ++a)
		{
			if (!UIRenderer::DebugStrings[a].hidden)
			{
				const XMVECTOR printLoc = XMVectorAdd(XMVectorMultiply(XMVectorSet(0, -.08f, 0, 0), XMVectorReplicate(static_cast<float>(a - skipCount))), XMVectorSet(-1, .92f, 0, 0));
				this->RenderString(UIRenderer::DebugStrings[a].text, XMMatrixTranslationFromVector(printLoc), XMVectorSet(0, 0, 0, 0));
			}
			else
			{
				++skipCount;
			}
		}
	}

	//Render the UI Tree from the top down compounding transformations
	void UIRenderer::renderComposedUITreeHelper(UIElement* element, const DirectX::XMMATRIX& compoundTransformation)
	{
		XMMATRIX myTransform = XMMatrixMultiply(XMMatrixMultiply(
			XMMatrixScalingFromVector(XMVectorSetZ(XMVectorSetW(element->GetBounds(), 1), 1)),
			//XMMatrixIdentity(),
			//XMMatrixRotationRollPitchYawFromVector(this->pitchRollYaw)), 
			XMMatrixRotationRollPitchYawFromVector(XMVectorSet(0, 0, element->rot, 1))),
			//XMMatrixIdentity()),
			XMMatrixTranslationFromVector(XMVectorSetW(element->GetPosition(), 1)));

		const XMMATRIX newTransformation = XMMatrixMultiply(myTransform, compoundTransformation);
		//const XMMATRIX newTransformation = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationZ(element->rot), XMMatrixTranslationFromVector(element->GetPosition())), compoundTransformation);

		//Render the children first so they appear on top of the parent
		//for(unsigned int whichChild = 0; whichChild < element->GetChildrenMap().size(); ++whichChild)
		for (auto el : element->GetChildrenMap())
		{
			this->renderComposedUITreeHelper(el.second, newTransformation);
		}

		//If the element is a visible type, render it
		if(element->IsRenderable() && element->IsVisible())
		{
			//Element color changes when focused.po
			if(element->HasFocus())
			{
				this->uiModelBufferData.tint = XMVectorAdd(element->GetForegroundTint(), element->GetFocusTint());
			}
			else
			{
				this->uiModelBufferData.tint = element->GetForegroundTint();
			}

			const XMMATRIX proj = XMMatrixMultiply(XMMatrixScalingFromVector(XMVectorSet(2, 2, 1, 1)), XMMatrixTranslationFromVector(XMVectorSet(0, 0, 0, 0)));
			const XMVECTOR composedScale = element->GetComposedBounds();
			const XMVECTOR recipricolScale = XMVectorReciprocal(composedScale);
			for (unsigned int a = 0; a < element->GetTextLabels().size(); ++a)
			{
				if (!element->GetTextLabels()[a].visible)
					continue;

				const string renderString = element->GetTextLabels()[a].text;
				this->RenderString(renderString, 
					XMMatrixMultiply(XMMatrixMultiply(XMMatrixMultiply(XMMatrixScalingFromVector(XMVectorScale(XMVectorMultiply(recipricolScale, element->GetTextLabels()[a].scale), .95f)), XMMatrixTranslationFromVector(element->GetTextLabels()[a].offset)), newTransformation), proj),//renderString,newTransformation,
					this->uiModelBufferData.tint);
			}

			this->uiModelBufferData.model = XMMatrixTranspose(XMMatrixMultiply(newTransformation, proj));
			this->deviceContext->UpdateSubresource(oncePerModelBuffer, 0, NULL, &uiModelBufferData, 0, 0);
			this->uiModel->modelShaderData.textureResourceView = element->GetTexture();
			this->uiModel->modelShaderData.samplerStateSlot = 1;
			this->uiModel->modelShaderData.resourceViewSlot = 1;
			Direct3DRenderer::RenderModel(this->uiModel);
		}
	}

	void UIRenderer::RenderString(const string text, const XMMATRIX& location, const XMVECTOR& tint)
	{
		const int textLength = static_cast<int>(text.length());
		if(this->defaultFontDefinition == nullptr || textLength == 0)
		{
			return;
		}

		float startPosition = 0;
		const float fontHeight = this->defaultFontDefinition->characterHeight / static_cast<float>(this->height);
		this->uiModelBufferData.model = XMMatrixTranspose(location);
		this->uiModelBufferData.tint = tint;
		//this->uiModelBufferData.scaleMatrix = XMMatrixTranspose(XMMatrixScaling(scaleFactor, scaleFactor, 1));
		this->deviceContext->UpdateSubresource(oncePerModelBuffer, 0, NULL, &uiModelBufferData, 0, 0);
		UIVertexPositionColor* const fontVertices = new UIVertexPositionColor[textLength * 4];

		for(int a = 0; a < textLength; ++a)
		{
			if(a >= textLength)
				break;
			const char characterIndex = text[min(a, text.length())];
			if(characterIndex == 32)
			{
				startPosition+= 3 * this->defaultFontDefinition->characterWidth / static_cast<float>(this->width);
				fontVertices[a*4].position = XMVectorSet(0, 0, 0, 0);
				fontVertices[a*4].texCoord[0] = 0;
				fontVertices[a*4].texCoord[1] = 0;

				fontVertices[a*4 + 1].position = XMVectorSet(0, 0, 0, 0);
				fontVertices[a*4 + 1].texCoord[0] = 0;
				fontVertices[a*4 + 1].texCoord[1] = 0;

				fontVertices[a*4 + 2].position = XMVectorSet(0, 0, 0, 0);
				fontVertices[a*4 + 2].texCoord[0] = 0;
				fontVertices[a*4 + 2].texCoord[1] = 0;

				fontVertices[a*4 + 3].position = XMVectorSet(0, 0, 0, 0);
				fontVertices[a*4 + 3].texCoord[0] = 0;
				fontVertices[a*4 + 2].texCoord[1] = 0;
			}
			else
			{
				const float charWidth = this->defaultFontDefinition->characterDefinitions[characterIndex].pixelWidth * this->defaultFontDefinition->characterWidth / this->width * 1.2f;
				const float endPosition = startPosition + charWidth;

				fontVertices[a*4 + 3].position = XMVectorSet(startPosition, 0, 0, 0);
				fontVertices[a*4 + 3].texCoord[0] = this->defaultFontDefinition->characterDefinitions[characterIndex].textureCoordinates[0];
				fontVertices[a*4 + 3].texCoord[1] = 1;

				fontVertices[a*4 + 2].position = XMVectorSet(startPosition, fontHeight, 0, 0);
				fontVertices[a*4 + 2].texCoord[0] = this->defaultFontDefinition->characterDefinitions[characterIndex].textureCoordinates[0];
				fontVertices[a*4 + 2].texCoord[1] = 0;

				fontVertices[a*4 + 1].position = XMVectorSet(endPosition, fontHeight, 0, 0);
				fontVertices[a*4 + 1].texCoord[0] = this->defaultFontDefinition->characterDefinitions[characterIndex].textureCoordinates[1];
				fontVertices[a*4 + 1].texCoord[1] = 0;

				fontVertices[a*4 + 0].position = XMVectorSet(endPosition, 0, 0, 0);
				fontVertices[a*4 + 0].texCoord[0] = this->defaultFontDefinition->characterDefinitions[characterIndex].textureCoordinates[1];
				fontVertices[a*4 + 0].texCoord[1] = 1;

				startPosition = endPosition;
				startPosition+= this->defaultFontDefinition->characterSpacing / static_cast<float>(this->width);
			}
		}

		D3D11_MAPPED_SUBRESOURCE ms;
		this->deviceContext->Map(this->textModel->bufferData->vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, fontVertices, sizeof(UIVertexPositionColor) * textLength * 4);
		this->deviceContext->Unmap(this->textModel->bufferData->vertexBuffer, NULL);

		delete [] fontVertices;
		Direct3DRenderer::RenderModel(this->textModel, textLength * 6);
	}

	void UIRenderer::initializeUIModel(const ModelShaderData& shaderData, D3D_FEATURE_LEVEL m_featureLevel)
	{
		ModelBufferData* const uiBufferData = new ModelBufferData();
		uiBufferData->indexOffset = 0;
		uiBufferData->vertexOffset = 0;
		uiBufferData->vertexSize = sizeof(UIVertexPositionColor);
		uiBufferData->indexFormat = DEFAULT_INDEX_FORMAT;
		uiBufferData->vertexCount = 4;
		uiBufferData->indexCount[0] = 6;

		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth = sizeof(UIVertexPositionColor) * 4;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		const UIVertexPositionColor vertices[4] = {
			{XMVectorSet(.5f, -.5f, 0, 0), {1,0}},
			{XMVectorSet(.5f, .5f, 0, 0), {1,1}},
			{XMVectorSet(-.5f, .5f, 0, 0), {0,1}},
			{XMVectorSet(-.5f, -.5f, 0, 0), {0,0}}};

		D3D11_SUBRESOURCE_DATA InitDataV;
		InitDataV.pSysMem = vertices;
		InitDataV.SysMemPitch = 0;
		InitDataV.SysMemSlicePitch = 0;
		if(FAILED(device->CreateBuffer(&bufferDesc, &InitDataV, &uiBufferData->vertexBuffer)))
		{
			assert(false);
			throw int();
		}

		D3D11_BUFFER_DESC bufferDesc2;
		bufferDesc2.Usage           = D3D11_USAGE_IMMUTABLE;
		bufferDesc2.ByteWidth = sizeof(shaderindextype) * 6;
		// Define the resource data.
		const shaderindextype indices[6] = {2, 1, 0, 0, 3, 2};
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		bufferDesc2.BindFlags       = D3D11_BIND_INDEX_BUFFER;
		bufferDesc2.CPUAccessFlags  = 0;
		bufferDesc2.MiscFlags       = 0;

		// Create the buffer with the device.
		if(FAILED(device->CreateBuffer(&bufferDesc2, &InitData, &uiBufferData->indexBuffer[0])))
		{
			throw int();
		}

		this->uiModel = new Model(uiBufferData, shaderData);
		uiModelPublic = uiBufferData;
	}

	FontDefinition* LoadFontDefinition(stringstream& inputFile)
	{
		FontDefinition* fontDefinition = NULL;

		if(inputFile.good())
		{
			fontDefinition = new FontDefinition();
			int index = 0;
			while(inputFile.good())
			{
				float texCoord1;
				float texCoord2;
				int pWidth;
				int wastedSpace;
				char wastedSpace2;
				inputFile >> wastedSpace;
				if(index > 0)
					inputFile >> wastedSpace2;
				inputFile >> texCoord1;
				inputFile >> texCoord2;
				inputFile >> pWidth;

				fontDefinition->characterDefinitions[wastedSpace].character = wastedSpace;
				fontDefinition->characterDefinitions[wastedSpace].textureCoordinates[0] = texCoord1;
				fontDefinition->characterDefinitions[wastedSpace].textureCoordinates[1] = texCoord2;
				fontDefinition->characterDefinitions[wastedSpace].pixelWidth = pWidth;
				++index;
			}
		}
		else
		{
			throw int();
		}

		return fontDefinition;
	}

	void UIRenderer::initializeFont()
	{
		HRESULT hr = 0;
		
		ifstream myFile(ResourceManager::BaseResourceDirectory + "Resources/Textures/Fonts/fontdata.txt", ios::ate);

		if(myFile.is_open())
		{
			const size_t fileSize = myFile.tellg();
			myFile.seekg(0, ios::beg);
			char* shaderData = new char[fileSize];
			myFile.read(shaderData, fileSize);
			myFile.close();
			const string inpString(shaderData);
			stringstream inpStream;
			inpStream << inpString;
			this->defaultFontDefinition = LoadFontDefinition(inpStream);
			delete[] shaderData;
		}

		if(this->defaultFontDefinition == NULL)
		{
			FailExit();
			return;
		}

		this->defaultFontDefinition->characterHeight = 48;
		this->defaultFontDefinition->characterWidth = 3;
		this->defaultFontDefinition->characterSpacing = 2;


		

		//this->deviceContext->PSSetSamplers(0, 1, &this->sampleState);
		//this->deviceContext->PSSetShaderResources(0, 1, &this->fontTextureView);
	}

	static const int MaxStringLength = 500;
	void UIRenderer::createTextBuffer(const ModelShaderData& textShaderData, D3D_FEATURE_LEVEL m_featureLevel)
	{
		this->textModel = new Model(new ModelBufferData, textShaderData);
		this->textModel->deleteBuffer = true;
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		textModel->bufferData->vertexCount = MaxStringLength * 4;
		textModel->bufferData->indexCount[0] = MaxStringLength * 6;
		bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
		bd.ByteWidth = sizeof(UIVertexPositionColor) * this->textModel->bufferData->vertexCount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

		if(FAILED(device->CreateBuffer(&bd, NULL, &this->textModel->bufferData->vertexBuffer)))      // create the buffer
		{
			throw int();
		}

		// Fill in a buffer description.
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage           = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags       = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags  = 0;
		bufferDesc.MiscFlags       = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		
		bufferDesc.ByteWidth = sizeof(shaderindextype) * this->textModel->bufferData->indexCount[0];
		shaderindextype indices[MaxStringLength * 6];
		for(unsigned int a = 0; a < MaxStringLength; ++a)
		{
			indices[a*6] = a*4 + 2;
			indices[a*6 + 1] = a*4 + 1;
			indices[a*6 + 2] = a*4 + 0;

			indices[a*6 + 3] = a*4 + 0;
			indices[a*6 + 4] = a*4 + 3;
			indices[a*6 + 5] = a*4 + 2;
		}

		InitData.pSysMem = indices;
		this->textModel->bufferData->indexFormat = DEFAULT_INDEX_FORMAT;

		

		// Create the buffer with the device.
		HRESULT hr = device->CreateBuffer(&bufferDesc, &InitData, &this->textModel->bufferData->indexBuffer[0]);
		if (FAILED(hr))
			throw int();

		this->textModel->bufferData->vertexSize = sizeof(UIVertexPositionColor);
		this->textModel->bufferData->indexOffset = 0;
		this->textModel->bufferData->vertexOffset = 0;
	}

	void UIRenderer::HideDebugString(int index)
	{
		/*if (index == DebugStrings.size() - 1)
		{
			DebugStrings.resize(DebugStrings.size() - 1);
		}
		else
		{
			DebugStrings[index] = DebugStrings[DebugStrings.size() - 1];
		}

		DebugStrings.resize(DebugStrings.size() - 1);*/
		DebugStrings[index].hidden = true;
	}

	UIRenderer::~UIRenderer()
	{
		if(this->uiModel != NULL)
		{
			delete this->uiModel;
		}

		if(this->textModel != NULL)
		{
			delete this->textModel;
		}
	}
}