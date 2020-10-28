#pragma once
#include "..\Rendering Engine Standard\StandardCubeRenderer.h"

namespace SimpleUI { class Window; }
namespace RenderingEngine
{
	class UIRenderer;
	
	//The SampleRenderer class is a nearly-complete renderer set up to use default options, default UI library, and
	//vertex / model types.  The purpose of this class is to prevent having to duplicate these options every
	//time a new project is created.  Most of the actual rendering code is in the StandardCubeRenderer class.
	class SampleRenderer : public StandardCubeRenderer
	{
	protected:
		FontDefinition* defaultFontDefinition = nullptr;
		UIRenderer* uiRenderer = nullptr;
		SimpleUI::Window* window = nullptr;

	public:
		SampleRenderer();
		~SampleRenderer();

		virtual void Render() override;

		virtual void InitializeRasterizerStates() override;

		bool initializeUI(int FontSamplerState, int UIPixelShader, int UIVertexShader, int UIInputLayout, int FontTexture, int FontPixelShader, int FontResourceView);
		template<class VertexType> Model* CreateStandardModel(int vertexShaderID, int pixelShaderID, int inputLayoutID, int vertexCount = 150000, int indexCount = 500000) const;
		template<class VertexType> ModelBufferData* CreateStandardBufferData(int vertexCount, int* indexCount, int indexBufferCount) const;
		template<class VertexType> ModelBufferData* CreateStandardBufferData(int vertexCount = 100000, int indexCount = 300000) const;
		template<class VertexType> void CreateStandardVertexBuffer(ModelBufferData* const bufferData, const int vertexCount) const;
		void CreateStandardIndexBuffers(ModelBufferData* const bufferData, const int* indexCount, const int indexBufferCount) const;
		ModelBufferData* CreateBufferDataFromString(const char* data, bool usePerVertexNormals = false, bool invertNormals = false) const;
		ModelBufferData* CreateBufferDataFromVertexData(int vertexCount, int indexCount, DirectX::XMVECTOR* vertices, DirectX::XMVECTOR* colors, DirectX::XMVECTOR* normals, shaderindextype* indices, DirectX::XMVECTOR* tangents = NULL, DirectX::XMVECTOR* binormals = NULL) const;
		void ParseFileData(const char* fileName, unsigned int& vertexCount, unsigned int& indexCount, DirectX::XMVECTOR** vertices, DirectX::XMVECTOR** colors, DirectX::XMVECTOR** normals, shaderindextype** indices, bool usePerVertexNormals = true, bool invertNormals = false) const;

		ModelBufferData* CreateQuadModel() const;

		ModelBufferData * CreateQuadModel2() const;

		void SetWindow(SimpleUI::Window* window) {this->window = window;}
	};
}