#pragma once
#include <d3d11.h>
#include "Direct3DBase.h"
#include <DirectXMath.h>

namespace Geometry {class Mesh;}

namespace RenderingEngine
{
	class ModelBufferData;
	class InstanceBufferData;
	struct ModelShaderData;
	struct Model;

	class Direct3DRenderer
	{
	private:
		static ID3D11VertexShader* currentVertexShader;
		static ID3D11PixelShader* currentPixelShader;
		static ID3D11Buffer* currentVertexBuffer[2];
		static ID3D11Buffer* currentIndexBuffer;
		static ID3D11InputLayout* currentInputLayout;
		static D3D11_PRIMITIVE_TOPOLOGY currentPrimitiveType;
		static std::vector<ID3D11ShaderResourceView*> currentShaderResourceViews;
		static std::vector<ID3D11SamplerState*> currentSamplerStates;
		static ID3D11DeviceContext* deviceContext;
		static ID3D11Device* device;

	public:
		static void RenderModel(const Model* model);
		static void RenderModel(const Model* model, int indexCount);
		static void RenderModel(const Model* model, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11InputLayout* inputLayout);
		static void RenderLODModel(const Model* model, const DirectX::XMVECTOR& cameraPosition);
		static void RenderDistanceLODModel(const Model* model, const DirectX::XMVECTOR& cameraPosition);
		static void SetCurrentVertexShader(ID3D11VertexShader* vertexShader);
		static void SetCurrentPixelShader(ID3D11PixelShader* pixelShader);
		static void SetCurrentVertexBuffer(ID3D11Buffer* vertexBuffer, unsigned int* vertexStride, unsigned int* vertexOffset);
		static void SetCurrentVertexBufferInstanced(ID3D11Buffer* vertexBuffer[2], unsigned int* vertexStride, unsigned int* vertexOffset);
		static void SetCurrentIndexBuffer(ID3D11Buffer* indexBuffer, DXGI_FORMAT indexFormat, unsigned int indexOffset);
		static void SetCurrentInputLayout(ID3D11InputLayout* inputLayout);
		static void SetCurrentPrimitiveType(const D3D11_PRIMITIVE_TOPOLOGY& primitiveType);
		static void SetCurrentShaderResourceViews(ID3D11ShaderResourceView* shaderResourceView, int index);
		static void SetCurrentSamplerState(ID3D11SamplerState* samplerState, int index);
		static void SetCurrentDeviceContext(ID3D11DeviceContext* deviceContext);
		static void SetCurrentDevice(ID3D11Device* device);

		template<class VertexType> static Model* CreateStaticModelFromMesh(Geometry::Mesh* mesh, const ModelShaderData& shaderData);
		template<class VertexType> static ModelBufferData* CreateStaticBufferDataFromMesh(Geometry::Mesh* mesh);
		template<class VertexType> static ModelBufferData* CreateStaticBufferDataFromMesh(Geometry::Mesh* mesh, int lodLevel);

	private:
		static void RenderBuffer(ModelBufferData* bufferData);
		static void RenderBuffer(ModelBufferData* bufferData, int indexCount, int vertexOffset = 0);
		static void RenderBufferInstanced(ModelBufferData* bufferData, int indexCount, int instanceCount);
		static void MakeModelDataCurrent(const Model* model, int bufferIndex = 0);
		static void MakeShaderDataCurrent(const ModelShaderData& shaderData);
		static void MakeBufferDataCurrent(ModelBufferData* bufferData, int bufferIndex = 0, InstanceBufferData* instanceBufferData = NULL);
	};
}