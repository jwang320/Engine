#pragma once
#include "ModelVertexTypes.h"
#include "..\Rendering Engine Base\Direct3DBase.h"
#include "..\Rendering Engine Base\ModelBufferData.h"

namespace SimulationEngine {class PhysicalWorldSimulation;}

namespace RenderingEngine
{
	class SceneGraph;

	//This class does most of the work needed for rendering.
	class StandardCubeRenderer : public Direct3DBase
	{
	public:
		enum DepthStencilStates
		{
			DefaultDepthStencilState,
			TransparencyDepthStencilState,
			DepthOffState
		};

		enum BlendStates
		{
			BlendOnState,
			BlendOffState
		};

	protected:
		std::string shaderDir;
		std::string textureDir;
		bool useHDR = true;
		SceneGraph* sceneGraph;

		ID3D11Buffer* oncePerResizeBuffer = nullptr;
		ID3D11Buffer* oncePerFrameBuffer = nullptr;
		ID3D11Buffer* oncePerModelBuffer = nullptr;

		ID3D11Buffer* oncePerModelBufferShadow;

		OncePerFrameConstantBuffer oncePerFrameBufferData;
		OncePerModelConstantBuffer oncePerModelBufferData;
		OncePerResizeConstantBuffer oncePerResizeBufferData;
		OncePerModelConstantBufferShadow oncePerModelBufferDataShadow;
		float aspectRatio;
		float fovAngleY;
		float nearZPlane;
		bool updateRasterizerState = false;

		bool wireFrame = false;

		ID3D11Texture2D* pShadowMap;
		ID3D11DepthStencilView* pShadowMapDepthView;
		ID3D11ShaderResourceView* pShadowMapSRView;

		ID3D11Texture2D* hdrTexture;
		ID3D11Texture2D* bloomTexture;
		ID3D11Texture2D* blurTexture;
		ID3D11Texture2D* blurTexture2;
		ID3D11Texture2D* luminanceTexture;
		ID3D11ShaderResourceView* prePassShaderResourceView;
		ID3D11ShaderResourceView* bloomShaderResourceView;
		ID3D11ShaderResourceView* blurShaderResourceView;
		ID3D11ShaderResourceView* blurShaderResourceView2;
		ID3D11ShaderResourceView* luminanceShaderResourceView;
		ID3D11RenderTargetView* prePassRenderTargetView;
		ID3D11RenderTargetView* bloomRenderTargetView;
		ID3D11RenderTargetView* blurRenderTargetView;
		ID3D11RenderTargetView* blurRenderTargetView2;
		ID3D11RenderTargetView* luminanceRenderTargetView;
		ID3D11PixelShader* shadowPixelShader;
		ID3D11PixelShader* hdrPixelShader;
		ID3D11PixelShader* blurPixelShader;
		ID3D11PixelShader* bloomCombinePixelShader;
		ID3D11PixelShader* luminancePixelShader;
		ID3D11VertexShader* shadowVertexShader;
		ID3D11VertexShader* hdrVertexShader;
		ID3D11InputLayout* shadowInputLayout;
		ID3D11InputLayout* hdrInputLayout;

		D3D11_VIEWPORT shadowViewPort;
		const float shadowWidth = 16384.f;
		ID3D11SamplerState* shadowSamplerState;
		ID3D11SamplerState* hdrSamplerState;
		ID3D11SamplerState* bloomSamplerState;
		ID3D11RasterizerState* shadowRasterizerState;
		Model* screenQuadModel;

	public:
		StandardCubeRenderer();
		virtual void Render();
		void ToggleWireFrame();
		float GetAspectRatio() const {return this->aspectRatio;}
		float GetFovAngleY() const {return this->fovAngleY;}
		float GetNearZPlane() const {return this->nearZPlane;}
		void ResetSceneGraph();
		OncePerFrameConstantBuffer GetConstantBufferData() const { return this->oncePerFrameBufferData; }
		OncePerResizeConstantBuffer GetProjectionBufferData() const { return this->oncePerResizeBufferData; }

		ModelShaderData CreateStandardModelShaderData(int vertexShaderID, int pixelShaderID, int inputLayoutID, int textureSlot = -1, int shaderResourceID = -1, int samplerStateID = -1, int shaderResourceSlot = -1, int samplerStateSlot = -1) const;
		ModelShaderData CreateStandardParticleShaderData(int vertexShaderID, int pixelShaderID, int inputLayoutID, int textureSlot = -1, int shaderResourceID = -1, int samplerStateID = -1, int shaderResourceSlot = -1, int samplerStateSlot = -1) const;
		SceneGraph* GetSceneGraph() const {return this->sceneGraph;}

		virtual ~StandardCubeRenderer() override;

	protected:
		virtual void InitializeOther() override;
		virtual void InitializeBuffers() override;
		virtual void InitializeProjection();

	private:
		void renderModels();
		void renderSceneToTexture(ID3D11RenderTargetView* renderTarget, int shaderResourceSlot);
		void renderSceneTextureToScreen(ID3D11ShaderResourceView* shaderResourceView, int shaderResourceSlot, Model* screenQuad);
		void renderScene(ID3D11RenderTargetView* renderTarget);
		void renderModelsShadowMap();
		void buildShadowMap();
		void renderParticles();
		void updateCamera();
		void makeRendererReady();
		void updateLights();
		void UpdateRasterizerState();
		void initializeDepthStencilStates();
		void initializeBlendStates();
		bool initializeRenderTargetsTemp();
		void initializeHDR();

	public:
		enum RasterizerStates
		{
			DefaultRasterizerState,
			WireframeRasterizerState,
			UIRasterizerState
		};
	};
}