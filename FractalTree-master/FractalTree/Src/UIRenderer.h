#pragma once
#include "..\Common Source\Rendering Engine\Rendering Engine Base\ModelBufferData.h"

#ifndef _NO_GRAPHICS
//#ifdef _WIN32_APPLICATION
// typedef ID3D11DeviceContext ID3D11DeviceContext1;
// typedef ID3D11Device ID3D11Device1;
//#endif

namespace SimpleUI {class UIElement;}
namespace SimpleUI {class Window;}

namespace RenderingEngine
{
	class Direct3DRenderer;
	struct Model;

	struct OncePerModelConstantBufferUI
	{
		DirectX::XMMATRIX model;
		DirectX::XMVECTOR tint;
	};

	struct DebugString
	{
		std::string text;
		bool hidden;
	};

	class UIRenderer
	{
	private:
		ID3D11DeviceContext* deviceContext;
		ID3D11Device* device;
		ID3D11Buffer* oncePerModelBuffer;
		OncePerModelConstantBufferUI uiModelBufferData;
		Model* uiModel = nullptr;
		Model* textModel = nullptr;
		Direct3DRenderer* modelRenderer;
		ID3D11ShaderResourceView* fontTextureView;
		ID3D11Resource* fontTexture;
		ID3D11SamplerState* sampleState;
		int width;
		int height;
		static std::vector<DebugString> DebugStrings; //Strings added to this will be printed out.

	public:
		static ModelBufferData* uiModelPublic;
		static FontDefinition* defaultFontDefinition;

		UIRenderer(ID3D11DeviceContext* deviceContext, ID3D11Device* device, ID3D11Buffer* dataBuffer, int windowWidth, int windowHeight,
			const ModelShaderData& uiShaderData,
			const ModelShaderData& fontShaderData, D3D_FEATURE_LEVEL m_featureLevel, ID3D11ShaderResourceView* fontTextureView = NULL, ID3D11Resource* fontTexture = NULL);
		void RenderComposedUITree(const SimpleUI::Window* window);
		void RenderString(const std::string text, const DirectX::XMMATRIX& location, const DirectX::XMVECTOR& tint);
		static int AddDebugString(const std::string& debugString) { UIRenderer::DebugStrings.push_back({ debugString, false }); return static_cast<int>(UIRenderer::DebugStrings.size()) - 1; }
		static void HideDebugString(int index);
		static void SetDebugString(int index, const std::string& debugString) { UIRenderer::DebugStrings[index] = { debugString, false }; }
		~UIRenderer();

	private:
		void renderComposedUITreeHelper(SimpleUI::UIElement* element, const DirectX::XMMATRIX& compoundTransformation);
		void initializeUIModel(const ModelShaderData& shaderData, D3D_FEATURE_LEVEL m_featureLevel);
		void createTextBuffer(const ModelShaderData& textShaderData, D3D_FEATURE_LEVEL m_featureLevel);
		void initializeFont();
	};
}
#endif