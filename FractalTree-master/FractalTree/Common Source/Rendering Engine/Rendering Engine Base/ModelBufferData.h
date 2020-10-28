#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

namespace RenderingEngine
{
	class ModelBufferData
	{
	public:
		ID3D11Buffer* vertexBuffer;
		unsigned int vertexCount;
		unsigned int vertexSize;
		unsigned int vertexOffset;

		int indexBufferCount;
		ID3D11Buffer** indexBuffer;
		unsigned int* indexCount;
		int* lodGridWidth;
		int* lodGridLength;
		int* lodGridFactor;

		float triangleWidth;
		int gridWidth;
		int gridLength;

		unsigned int indexOffset;
		DXGI_FORMAT indexFormat;
		unsigned int maxVertexCount;
		unsigned int maxIndexCount;

		ModelBufferData(int indexBufferCount = 1);
		void ReleaseBuffers();
		void ReleaseVertexBuffers();
		void ReleaseIndexBuffers();
		~ModelBufferData();
	};

	class InstanceBufferData
	{
	public:
		ID3D11Buffer* instanceBuffer;
		unsigned int instanceCount;
		unsigned int instanceSize;
		unsigned int instanceOffset;
		InstanceBufferData();
		~InstanceBufferData();
	};

	struct TextureData
	{
		int resourceViewSlot;
		int samplerStateSlot;
		ID3D11ShaderResourceView* textureResourceView;
		ID3D11SamplerState* textureSamplerState;
		ID3D11Texture2D* texture;
	};

	struct ModelShaderData
	{
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		ID3D11InputLayout* inputLayout;
		D3D11_PRIMITIVE_TOPOLOGY primitiveType;
		int resourceViewSlot;
		int samplerStateSlot;
		ID3D11ShaderResourceView* textureResourceView;
		ID3D11SamplerState* textureSamplerState;
		ID3D11Texture2D* texture;
		ID3D11ShaderResourceView* alphaMap;
		ID3D11Texture2D* alphaMapTexture;

		ModelShaderData(ID3D11VertexShader* vertexShader = NULL,
			ID3D11PixelShader* pixelShader = NULL,
			ID3D11InputLayout* inputLayout = NULL,
			D3D11_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,
			int resourceViewSlot = -1,
			int samplerStateSlot = -1,
			ID3D11ShaderResourceView* textureResourceView = NULL,
			ID3D11SamplerState* textureSamplerState = NULL,
			ID3D11Texture2D* texture = NULL,
			ID3D11ShaderResourceView* alphaMap = NULL,
			ID3D11Texture2D* alphaMapTexture = NULL) :
		vertexShader(vertexShader),
			pixelShader(pixelShader),
			inputLayout(inputLayout),
			primitiveType(primitiveType),
			resourceViewSlot(resourceViewSlot),
			samplerStateSlot(samplerStateSlot),
			textureResourceView(textureResourceView),
			textureSamplerState(textureSamplerState),
			texture(texture),
			alphaMap(alphaMap),
			alphaMapTexture(alphaMapTexture)
		{

		}
	};

	enum ModelType
	{
		ModelTypeStandard,
		ModelTypeLOD,
		ModelTypeGrid
	};

	struct Model
	{
		ModelBufferData* bufferData;
		ModelShaderData modelShaderData;
		ModelType modelType;
		int lodDepth;
		bool deleteBuffer;
		bool renderLOD;
		int activeBufferIndex;
		bool checkBounds;
		InstanceBufferData* instanceBufferData;
		bool castsShadow;
		DirectX::XMVECTOR tint;
		DirectX::XMVECTOR diffuseTint;
		DirectX::XMVECTOR customValue;
		bool isLineMesh = false;
		/*Model() :
			lodDepth(1),
			deleteBuffer(false),
			renderLOD(false),
			activeBufferIndex(0),
			checkBounds(false)
		{}*/
		Model(ModelBufferData* bufferData, const ModelShaderData& modelShaderData, int lodDepth = 1, ModelType modelType = ModelTypeStandard, InstanceBufferData* instanceBufferData = NULL) :
			bufferData(bufferData),
			modelShaderData(modelShaderData),
			lodDepth(lodDepth),
			modelType(modelType),
			deleteBuffer(true),
			renderLOD(false),
			activeBufferIndex(0),
			checkBounds(false),
			instanceBufferData(instanceBufferData),
			castsShadow(false),
			tint(DirectX::XMVectorSet(0, 0, 0, 0)),
			diffuseTint(DirectX::XMVectorSet(1, 1, 1, 1)),
			customValue(DirectX::XMVectorSet(0, 0, 0, 0))
		{

		}
		~Model();
	};
}