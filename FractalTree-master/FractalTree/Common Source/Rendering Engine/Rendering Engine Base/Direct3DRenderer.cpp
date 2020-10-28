#include "pch.h"
#include "Direct3DRenderer.h"
#include "../Rendering Engine Standard/SceneGraph.h"
#include "ModelBufferData.h"

using namespace DirectX;

namespace RenderingEngine
{
	ID3D11VertexShader* Direct3DRenderer::currentVertexShader = NULL;
	ID3D11PixelShader* Direct3DRenderer::currentPixelShader = NULL;
	ID3D11Buffer* Direct3DRenderer::currentVertexBuffer[2] = {NULL, NULL};
	ID3D11Buffer* Direct3DRenderer::currentIndexBuffer = NULL;
	ID3D11InputLayout* Direct3DRenderer::currentInputLayout = NULL;
	D3D11_PRIMITIVE_TOPOLOGY Direct3DRenderer::currentPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	ID3D11DeviceContext* Direct3DRenderer::deviceContext = NULL;
	ID3D11Device* Direct3DRenderer::device = NULL;
	std::vector<ID3D11ShaderResourceView*> Direct3DRenderer::currentShaderResourceViews;
	std::vector<ID3D11SamplerState*> Direct3DRenderer::currentSamplerStates;

	void Direct3DRenderer::RenderModel(const Model* model)
	{
		Direct3DRenderer::MakeModelDataCurrent(model, model->activeBufferIndex);
		if(model->instanceBufferData != NULL)
		{
			Direct3DRenderer::RenderBufferInstanced(model->bufferData, model->bufferData->indexCount[0], model->instanceBufferData->instanceCount);
		}
		else
		{
			Direct3DRenderer::RenderBuffer(model->bufferData, model->bufferData->indexCount[model->activeBufferIndex]);
		}	
	}

	void Direct3DRenderer::RenderModel(const Model* model, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11InputLayout* inputLayout)
	{
		Direct3DRenderer::SetCurrentVertexShader(vertexShader);
		Direct3DRenderer::SetCurrentPixelShader(pixelShader);
		Direct3DRenderer::SetCurrentInputLayout(inputLayout);
		Direct3DRenderer::SetCurrentPrimitiveType(model->modelShaderData.primitiveType);
		Direct3DRenderer::MakeBufferDataCurrent(model->bufferData, model->activeBufferIndex, model->instanceBufferData);

		if(model->instanceBufferData != NULL)
		{
			Direct3DRenderer::RenderBufferInstanced(model->bufferData, model->bufferData->indexCount[0], model->instanceBufferData->instanceCount);
		}
		else
		{
			Direct3DRenderer::RenderBuffer(model->bufferData, model->bufferData->indexCount[model->activeBufferIndex]);
		}	
	}

	void Direct3DRenderer::RenderModel(const Model* model, int indexCount)
	{
		Direct3DRenderer::MakeModelDataCurrent(model, model->activeBufferIndex);
		Direct3DRenderer::RenderBuffer(model->bufferData, indexCount);
	}

	void Direct3DRenderer::RenderBuffer(ModelBufferData* bufferData)
	{
		if(bufferData->indexCount > 0)
		{
			Direct3DRenderer::deviceContext->DrawIndexed(bufferData->indexCount[0], 0, 0);
		}
	}

	void Direct3DRenderer::RenderBuffer(ModelBufferData* bufferData, int indexCount, int vertexOffset)
	{
		if(indexCount > 0)
		{
			Direct3DRenderer::deviceContext->DrawIndexed(indexCount, 0, vertexOffset);
		}
	}

	void Direct3DRenderer::RenderBufferInstanced(ModelBufferData* bufferData, int indexCount, int instanceCount)
	{
		if(indexCount > 0 && instanceCount > 0)
		{
			Direct3DRenderer::deviceContext->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
		}
	}

	void Direct3DRenderer::MakeModelDataCurrent(const Model* model, int bufferIndex)
	{
		Direct3DRenderer::MakeShaderDataCurrent(model->modelShaderData);
		Direct3DRenderer::MakeBufferDataCurrent(model->bufferData, bufferIndex, model->instanceBufferData);
	}

	void Direct3DRenderer::MakeShaderDataCurrent(const ModelShaderData& shaderData)
	{
		Direct3DRenderer::SetCurrentInputLayout(shaderData.inputLayout);
		Direct3DRenderer::SetCurrentVertexShader(shaderData.vertexShader);
		Direct3DRenderer::SetCurrentPixelShader(shaderData.pixelShader);
		Direct3DRenderer::SetCurrentPrimitiveType(shaderData.primitiveType);
		Direct3DRenderer::SetCurrentSamplerState(shaderData.textureSamplerState, shaderData.samplerStateSlot);
		Direct3DRenderer::SetCurrentShaderResourceViews(shaderData.textureResourceView, shaderData.resourceViewSlot);
		Direct3DRenderer::SetCurrentShaderResourceViews(shaderData.alphaMap, 3);
	}

	void Direct3DRenderer::MakeBufferDataCurrent(ModelBufferData* bufferData, int bufferIndex, InstanceBufferData* instanceBufferData)
	{
		if(instanceBufferData == NULL)
		{
			Direct3DRenderer::SetCurrentVertexBuffer(bufferData->vertexBuffer, &(bufferData->vertexSize), &bufferData->vertexOffset);
		}
		else
		{
			unsigned int vertexSize[2] = {bufferData->vertexSize, instanceBufferData->instanceSize};
			unsigned int vertexOffset[2] = {bufferData->vertexOffset, instanceBufferData->instanceOffset};
			ID3D11Buffer* buffers[2] = {bufferData->vertexBuffer, instanceBufferData->instanceBuffer};
			Direct3DRenderer::SetCurrentVertexBufferInstanced(buffers, vertexSize, vertexOffset);
		}

		Direct3DRenderer::SetCurrentIndexBuffer(bufferData->indexBuffer[bufferIndex], bufferData->indexFormat, bufferData->indexOffset);
	}

	void Direct3DRenderer::SetCurrentVertexShader(ID3D11VertexShader* vertexShader)
	{
		if(Direct3DRenderer::currentVertexShader != vertexShader)
		{
			Direct3DRenderer::currentVertexShader = vertexShader;
			Direct3DRenderer::deviceContext->VSSetShader(Direct3DRenderer::currentVertexShader, NULL, 0);
		}
	}

	void Direct3DRenderer::SetCurrentPixelShader(ID3D11PixelShader* pixelShader)
	{
		if(Direct3DRenderer::currentPixelShader != pixelShader)
		{
			Direct3DRenderer::currentPixelShader = pixelShader;
			Direct3DRenderer::deviceContext->PSSetShader(Direct3DRenderer::currentPixelShader, NULL, 0);
		}
	}

	void Direct3DRenderer::SetCurrentVertexBuffer(ID3D11Buffer* vertexBuffer, unsigned int* vertexStride, unsigned int* vertexOffset)
	{
		if(Direct3DRenderer::currentVertexBuffer[0] != vertexBuffer)
		{
			Direct3DRenderer::currentVertexBuffer[0] = vertexBuffer;
			Direct3DRenderer::deviceContext->IASetVertexBuffers(0, 1, Direct3DRenderer::currentVertexBuffer, vertexStride, vertexOffset);
		}
	}

	void Direct3DRenderer::SetCurrentVertexBufferInstanced(ID3D11Buffer* vertexBuffer[2], unsigned int* vertexStride, unsigned int* vertexOffset)
	{
		if(Direct3DRenderer::currentVertexBuffer != vertexBuffer)
		{
			Direct3DRenderer::currentVertexBuffer[0] = vertexBuffer[0];
			Direct3DRenderer::currentVertexBuffer[1] = vertexBuffer[1];
			Direct3DRenderer::deviceContext->IASetVertexBuffers(0, 2, vertexBuffer, vertexStride, vertexOffset);
		}
	}

	void Direct3DRenderer::SetCurrentIndexBuffer(ID3D11Buffer* indexBuffer, DXGI_FORMAT indexFormat, unsigned int indexOffset)
	{
		if(Direct3DRenderer::currentIndexBuffer != indexBuffer)
		{
			Direct3DRenderer::currentIndexBuffer = indexBuffer;
			Direct3DRenderer::deviceContext->IASetIndexBuffer(Direct3DRenderer::currentIndexBuffer, indexFormat, indexOffset);
		}
	}

	void Direct3DRenderer::SetCurrentInputLayout(ID3D11InputLayout* inputLayout)
	{
		if(Direct3DRenderer::currentInputLayout != inputLayout)
		{
			Direct3DRenderer::currentInputLayout = inputLayout;
			Direct3DRenderer::deviceContext->IASetInputLayout(Direct3DRenderer::currentInputLayout);
		}
	}

	void Direct3DRenderer::SetCurrentPrimitiveType(const D3D11_PRIMITIVE_TOPOLOGY& primitiveType)
	{
		if(Direct3DRenderer::currentPrimitiveType != primitiveType)
		{
			Direct3DRenderer::currentPrimitiveType = primitiveType;
			Direct3DRenderer::deviceContext->IASetPrimitiveTopology(Direct3DRenderer::currentPrimitiveType);
		}
	}

	void Direct3DRenderer::SetCurrentShaderResourceViews(ID3D11ShaderResourceView* shaderResourceView, int index)
	{
		if(index < 0 || shaderResourceView == NULL)
		{
			return;
		}

		if(Direct3DRenderer::currentShaderResourceViews.size() < index + 1)
		{
			Direct3DRenderer::currentShaderResourceViews.resize(index + 1, NULL);
		}

		if(Direct3DRenderer::currentShaderResourceViews[index] != shaderResourceView)
		{
			Direct3DRenderer::currentShaderResourceViews[index] = shaderResourceView;
			Direct3DRenderer::deviceContext->PSSetShaderResources(index, 1, &shaderResourceView);
		}
	}

	void Direct3DRenderer::SetCurrentSamplerState(ID3D11SamplerState* samplerState, int index)
	{
		if(index < 0 || samplerState == NULL)
		{
			return;
		}

		if(Direct3DRenderer::currentSamplerStates.size() < index + 1)
		{
			Direct3DRenderer::currentSamplerStates.resize(index + 1, NULL);
		}

		if(Direct3DRenderer::currentSamplerStates[index] != samplerState)
		{
			Direct3DRenderer::currentSamplerStates[index] = samplerState;
			Direct3DRenderer::deviceContext->PSSetSamplers(index, 1, &samplerState);
		}
	}

	void Direct3DRenderer::SetCurrentDeviceContext(ID3D11DeviceContext* deviceContext)
	{
		Direct3DRenderer::deviceContext = deviceContext;
	}

	void Direct3DRenderer::SetCurrentDevice(ID3D11Device* device)
	{
		Direct3DRenderer::device = device;
	}

	void Direct3DRenderer::RenderLODModel(const Model* model, const DirectX::XMVECTOR& cameraPosition)
	{
		int bufferIndex = 1;
		int indCount = model->bufferData->indexCount[bufferIndex];
		if(indCount <= 0)
		{
			return;
		}

		Direct3DRenderer::MakeModelDataCurrent(model, bufferIndex);
		int startVertex;
		XMFLOAT4 posF;
		XMStoreFloat4(&posF, cameraPosition);
		int startX = static_cast<int>(posF.x / model->bufferData->triangleWidth) + model->bufferData->gridLength / 2 - model->bufferData->lodGridWidth[bufferIndex] / 2;
		int startZ = static_cast<int>(posF.z / model->bufferData->triangleWidth) + model->bufferData->gridLength / 2 - model->bufferData->lodGridLength[bufferIndex] / 2;
		
		{
			int xOffset = startX % 2;
			int zOffset = startZ % 2;

			startVertex = (startX - xOffset) + (startZ - zOffset) * model->bufferData->gridLength;
			Direct3DRenderer::deviceContext->DrawIndexed(indCount, 0, startVertex);
		}

		++bufferIndex;
		{
			int startX2 = startX + model->bufferData->lodGridWidth[bufferIndex] - model->bufferData->lodGridFactor[bufferIndex];
			int startZ2 = startZ;
			int xOffset = (startX2) % model->bufferData->lodGridFactor[bufferIndex];
			int zOffset = startZ2 % model->bufferData->lodGridFactor[bufferIndex];
			int startVertex2 = startX2 + (startZ2 - zOffset)*model->bufferData->gridLength - xOffset;
			indCount = model->bufferData->indexCount[bufferIndex];
			Direct3DRenderer::MakeModelDataCurrent(model, bufferIndex);
			Direct3DRenderer::deviceContext->DrawIndexed(indCount, 0, startVertex2);
		}
		{
			int startX2 = startX - (model->bufferData->lodGridWidth[bufferIndex] - model->bufferData->lodGridFactor[bufferIndex]);
			int startZ2 = startZ;
			int xOffset = (startX2) % model->bufferData->lodGridFactor[bufferIndex];
			int zOffset = startZ2 % model->bufferData->lodGridFactor[bufferIndex];
			int startVertex2 = startX2 + (startZ2 - zOffset)*model->bufferData->gridLength - xOffset;

			indCount = model->bufferData->indexCount[bufferIndex];
			Direct3DRenderer::MakeModelDataCurrent(model, bufferIndex);
			Direct3DRenderer::deviceContext->DrawIndexed(indCount, 0, startVertex2);
		}
		{

			int startX2 = startX;
			int startZ2 = startZ - (model->bufferData->lodGridLength[bufferIndex] - model->bufferData->lodGridFactor[bufferIndex]);
			int xOffset = (startX2) % model->bufferData->lodGridFactor[bufferIndex];
			int zOffset = startZ2 % model->bufferData->lodGridFactor[bufferIndex];
			int startVertex2 = startX2 + (startZ2 - zOffset)*model->bufferData->gridLength - xOffset;

			indCount = model->bufferData->indexCount[bufferIndex];
			Direct3DRenderer::MakeModelDataCurrent(model, bufferIndex);
			Direct3DRenderer::deviceContext->DrawIndexed(indCount, 0, startVertex2);
		}

		{

			int startX2 = startX;
			int startZ2 = startZ + (model->bufferData->lodGridLength[bufferIndex] - model->bufferData->lodGridFactor[bufferIndex]);
			int xOffset = (startX2) % model->bufferData->lodGridFactor[bufferIndex];
			int zOffset = startZ2 % model->bufferData->lodGridFactor[bufferIndex];
			int startVertex2 = startX2 + (startZ2 - zOffset)*model->bufferData->gridLength - xOffset;

			indCount = model->bufferData->indexCount[bufferIndex];
			Direct3DRenderer::MakeModelDataCurrent(model, bufferIndex);
			Direct3DRenderer::deviceContext->DrawIndexed(indCount, 0, startVertex2);
		}

		{

			int startX2 = startX - (model->bufferData->lodGridWidth[bufferIndex] - model->bufferData->lodGridFactor[bufferIndex]);
			int startZ2 = startZ - (model->bufferData->lodGridLength[bufferIndex] - model->bufferData->lodGridFactor[bufferIndex]);
			int xOffset = (startX2) % model->bufferData->lodGridFactor[bufferIndex];
			int zOffset = startZ2 % model->bufferData->lodGridFactor[bufferIndex];
			int startVertex2 = startX2 + (startZ2 - zOffset)*model->bufferData->gridLength - xOffset;

			indCount = model->bufferData->indexCount[bufferIndex];
			Direct3DRenderer::MakeModelDataCurrent(model, bufferIndex);
			Direct3DRenderer::deviceContext->DrawIndexed(indCount, 0, startVertex2);
		}
		
		{

			int startX2 = startX + (model->bufferData->lodGridWidth[bufferIndex] - model->bufferData->lodGridFactor[bufferIndex]);
			int startZ2 = startZ - (model->bufferData->lodGridLength[bufferIndex] - model->bufferData->lodGridFactor[bufferIndex]);
			int xOffset = (startX2) % model->bufferData->lodGridFactor[bufferIndex];
			int zOffset = startZ2 % model->bufferData->lodGridFactor[bufferIndex];
			int startVertex2 = startX2 + (startZ2 - zOffset)*model->bufferData->gridLength - xOffset;

			indCount = model->bufferData->indexCount[bufferIndex];
			Direct3DRenderer::MakeModelDataCurrent(model, bufferIndex);
			Direct3DRenderer::deviceContext->DrawIndexed(indCount, 0, startVertex2);
		}

		{

			int startX2 = startX - (model->bufferData->lodGridWidth[bufferIndex] - model->bufferData->lodGridFactor[bufferIndex]);
			int startZ2 = startZ + (model->bufferData->lodGridLength[bufferIndex] - model->bufferData->lodGridFactor[bufferIndex]);
			int xOffset = (startX2) % model->bufferData->lodGridFactor[bufferIndex];
			int zOffset = startZ2 % model->bufferData->lodGridFactor[bufferIndex];
			int startVertex2 = startX2 + (startZ2 - zOffset)*model->bufferData->gridLength - xOffset;

			indCount = model->bufferData->indexCount[bufferIndex];
			Direct3DRenderer::MakeModelDataCurrent(model, bufferIndex);
			Direct3DRenderer::deviceContext->DrawIndexed(indCount, 0, startVertex2);
		}

		{

			int startX2 = startX + (model->bufferData->lodGridWidth[bufferIndex] - model->bufferData->lodGridFactor[bufferIndex]);
			int startZ2 = startZ + (model->bufferData->lodGridLength[bufferIndex] - model->bufferData->lodGridFactor[bufferIndex]);
			int xOffset = (startX2) % model->bufferData->lodGridFactor[bufferIndex];
			int zOffset = startZ2 % model->bufferData->lodGridFactor[bufferIndex];
			int startVertex2 = startX2 + (startZ2 - zOffset)*model->bufferData->gridLength - xOffset;

			indCount = model->bufferData->indexCount[bufferIndex];
			Direct3DRenderer::MakeModelDataCurrent(model, bufferIndex);
			Direct3DRenderer::deviceContext->DrawIndexed(indCount, 0, startVertex2);
		}
	}

	void Direct3DRenderer::RenderDistanceLODModel(const Model* model, const DirectX::XMVECTOR& cameraPosition)
	{
		int bufferIndex = 1;
		int indCount = model->bufferData->indexCount[bufferIndex];
		if(indCount <= 0)
		{
			return;
		}

		Direct3DRenderer::MakeModelDataCurrent(model, bufferIndex);
		
	}

}