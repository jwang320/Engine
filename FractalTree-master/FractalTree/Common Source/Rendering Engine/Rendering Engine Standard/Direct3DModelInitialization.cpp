#include "pch.h"
#include "..\Rendering Engine Standard\ModelVertexTypes.h"
#include "..\Common Source\Mesh\Mesh.h"
#include "..\Rendering Engine Base\Direct3DRenderer.h"
#include "..\Rendering Engine Base\ModelBufferData.h"

using namespace DirectX;
using namespace Geometry;

namespace RenderingEngine
{
	template Model* Direct3DRenderer::CreateStaticModelFromMesh<TexturedVertex>(Mesh* mesh, const ModelShaderData& shaderData);

	template<class VertexType> Model* Direct3DRenderer::CreateStaticModelFromMesh(Mesh* mesh, const ModelShaderData& shaderData)
	{
		Model* model = new Model(Direct3DRenderer::CreateStaticBufferDataFromMesh<VertexType>(mesh),
			shaderData,
			mesh->GetLODLevel());
		
		return model;
	}

	template<class VertexType> ModelBufferData* Direct3DRenderer::CreateStaticBufferDataFromMesh(Mesh* mesh)
	{
		ModelBufferData* const bufferData = new ModelBufferData(mesh->GetLODLevel());

		if(mesh->GetVertexCount() <= 0)
		{
			return nullptr;
		}

		bufferData->vertexCount = mesh->requiredVertexCount;
		VertexType* const vertices = new VertexType[bufferData->vertexCount];

		for(unsigned int a = 0; a < static_cast<unsigned int>(mesh->GetVertexCount()); ++a)
		{
			vertices[a] = { mesh->GetVertices()[a],
				mesh->GetNormals()[a],
				mesh->GetTangents()[a],
				mesh->GetBinormals()[a],
				mesh->GetTexCoords()[a] };
		}

		for(int lodLevel = 0; lodLevel < mesh->lodLevel; ++lodLevel)
		{
			const unsigned int indCount = static_cast<unsigned int>(mesh->GetIndices(lodLevel).size());
			bufferData->indexCount[lodLevel] = indCount;
			if (indCount <= 3 || indCount % 3 != 0)
			{
				return nullptr;
			}
			const unsigned int triangleCount = indCount / 3;
				
			shaderindextype* const indices = new shaderindextype[indCount];
			for(unsigned int whichTriangle = 0; whichTriangle < triangleCount; ++whichTriangle)
			{
				if(mesh->faceData.useFaceNormals[whichTriangle])
				{
					const shaderindextype newIndex1 = static_cast<shaderindextype>(mesh->GetVertexCount()) + whichTriangle*3;
					const shaderindextype newIndex2 = static_cast<shaderindextype>(mesh->GetVertexCount()) + whichTriangle*3 + 1;
					const shaderindextype newIndex3 = static_cast<shaderindextype>(mesh->GetVertexCount()) + whichTriangle*3 + 2;
					vertices[newIndex1] = {mesh->vertex[mesh->indices[lodLevel][whichTriangle * 3]],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->texCoord[mesh->indices[lodLevel][whichTriangle * 3]]};

					vertices[newIndex2] = {mesh->vertex[mesh->indices[lodLevel][whichTriangle * 3 + 1]],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->texCoord[mesh->indices[lodLevel][whichTriangle * 3 + 1]]};

					vertices[newIndex3] = {mesh->vertex[mesh->indices[lodLevel][whichTriangle * 3 + 2]],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->texCoord[mesh->indices[lodLevel][whichTriangle * 3 + 1]]};

					indices[whichTriangle*3] = newIndex1;
					indices[whichTriangle*3 + 1] = newIndex2;
					indices[whichTriangle*3 + 2] = newIndex3;
				}
				else
				{
					indices[whichTriangle * 3] = mesh->indices[lodLevel][whichTriangle * 3];
					indices[whichTriangle * 3 + 1] = mesh->indices[lodLevel][whichTriangle * 3 + 1];
					indices[whichTriangle * 3 + 2] = mesh->indices[lodLevel][whichTriangle * 3 + 2];
				}
			}

			D3D11_BUFFER_DESC bufferDesc2;
			bufferDesc2.Usage = D3D11_USAGE_IMMUTABLE;
			
			bufferDesc2.ByteWidth = sizeof(shaderindextype) * bufferData->indexCount[lodLevel];
			
			bufferDesc2.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc2.CPUAccessFlags = 0;
			bufferDesc2.MiscFlags = 0;

			// Define the resource data.
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = indices;
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;

			// Create the buffer with the device.
			if(FAILED(device->CreateBuffer(&bufferDesc2, &InitData, &bufferData->indexBuffer[lodLevel])))
			{
				throw int();
			}

			delete[] indices;
		}


		bufferData->indexFormat = DEFAULT_INDEX_FORMAT;
		bufferData->vertexSize = sizeof(VertexType);
		bufferData->indexOffset = 0;
		bufferData->vertexOffset = 0;

		D3D11_BUFFER_DESC bufferDesc2;
		bufferDesc2.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc2.ByteWidth = bufferData->vertexSize * bufferData->vertexCount;
		bufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc2.CPUAccessFlags = 0;
		bufferDesc2.MiscFlags = 0;

		// Define the resource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		
		// Create the buffer with the device.
		if(FAILED(device->CreateBuffer(&bufferDesc2, &InitData, &bufferData->vertexBuffer)))
		{
			throw int();
		}

		delete[] vertices;
		return bufferData;
	}

	template<class VertexType> ModelBufferData* Direct3DRenderer::CreateStaticBufferDataFromMesh(Mesh* mesh1, int lodLevel)
	{
		UniformHeightMapMesh* mesh = static_cast<UniformHeightMapMesh*>(mesh1);
		ModelBufferData* bufferData = new ModelBufferData(mesh->GetLODLevel());

		if(mesh->GetVertexCount() <= 0)
		{
			return NULL;
		}

		int additionalVertexCount = mesh->requiredVertexCount - mesh->GetVertexCount();
		bufferData->vertexCount = mesh->requiredVertexCount;
		VertexType* vertices = new VertexType[bufferData->vertexCount];


		for(unsigned int a = 0; a < static_cast<unsigned int>(mesh->GetVertexCount()); ++a)
		{
			VertexType tt = {mesh->GetVertices()[a],
				mesh->GetNormals()[a],
				mesh->GetTangents()[a],
				mesh->GetBinormals()[a],
				mesh->GetTexCoords()[a]};
			vertices[a] = tt;
		}

		for(int lodLevel = 0; lodLevel < mesh->lodLevel; ++lodLevel)
		{
			unsigned int indCount = static_cast<unsigned int>(mesh->GetIndices(lodLevel).size());
			bufferData->indexCount[lodLevel] = indCount;
			unsigned int triangleCount = static_cast<int>(mesh->GetIndices(lodLevel).size()) / 3;

			shaderindextype* indices = new shaderindextype[bufferData->indexCount[lodLevel]];
			for(unsigned int whichTriangle = 0; whichTriangle < triangleCount; ++whichTriangle)
			{
				if(mesh->faceData.useFaceNormals[whichTriangle])
				{
					shaderindextype newIndex1 = static_cast<shaderindextype>(mesh->GetVertexCount()) + whichTriangle*3;
					shaderindextype newIndex2 = static_cast<shaderindextype>(mesh->GetVertexCount()) + whichTriangle*3 + 1;
					shaderindextype newIndex3 = static_cast<shaderindextype>(mesh->GetVertexCount()) + whichTriangle*3 + 2;
					VertexType tt = {mesh->vertex[mesh->indices[lodLevel][whichTriangle * 3]],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->texCoord[mesh->indices[lodLevel][whichTriangle * 3]]};

					VertexType tt1 = {mesh->vertex[mesh->indices[lodLevel][whichTriangle * 3 + 1]],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->texCoord[mesh->indices[lodLevel][whichTriangle * 3 + 1]]};

					VertexType tt2 = {mesh->vertex[mesh->indices[lodLevel][whichTriangle * 3 + 2]],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->faceData.faceNormals[whichTriangle],
						mesh->texCoord[mesh->indices[lodLevel][whichTriangle * 3 + 1]]};

					vertices[newIndex1] = tt;
					vertices[newIndex2] = tt1;
					vertices[newIndex3] = tt2;

					indices[whichTriangle*3] = newIndex1;
					indices[whichTriangle*3 + 1] = newIndex2;
					indices[whichTriangle*3 + 2] = newIndex3;
				}
				else
				{
					indices[whichTriangle * 3] = mesh->indices[lodLevel][whichTriangle * 3];
					indices[whichTriangle * 3 + 1] = mesh->indices[lodLevel][whichTriangle * 3 + 1];
					indices[whichTriangle * 3 + 2] = mesh->indices[lodLevel][whichTriangle * 3 + 2];
				}
			}

			D3D11_BUFFER_DESC bufferDesc2;
			bufferDesc2.Usage = D3D11_USAGE_IMMUTABLE;

			bufferDesc2.ByteWidth = sizeof(shaderindextype) * bufferData->indexCount[lodLevel];

			bufferDesc2.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc2.CPUAccessFlags = 0;
			bufferDesc2.MiscFlags = 0;

			// Define the resource data.
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = indices;
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;

			// Create the buffer with the device.
			if(FAILED(device->CreateBuffer(&bufferDesc2, &InitData, &bufferData->indexBuffer[lodLevel])))
			{
				throw int();
			}

			delete[] indices;
		}


		bufferData->indexFormat = DEFAULT_INDEX_FORMAT;
		bufferData->vertexSize = sizeof(VertexType);
		bufferData->indexOffset = 0;
		bufferData->vertexOffset = 0;

		D3D11_BUFFER_DESC bufferDesc2;
		bufferDesc2.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc2.ByteWidth = bufferData->vertexSize * bufferData->vertexCount;
		bufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc2.CPUAccessFlags = 0;
		bufferDesc2.MiscFlags = 0;

		// Define the resource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		// Create the buffer with the device.
		if(FAILED(device->CreateBuffer(&bufferDesc2, &InitData, &bufferData->vertexBuffer)))
		{
			assert(false);
			throw int();
		}

		delete[] vertices;
		return bufferData;
	}
}