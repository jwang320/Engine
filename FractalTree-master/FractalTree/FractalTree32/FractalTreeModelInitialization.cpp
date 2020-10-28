#include "pch.h"
#include "FractalTreeRenderer.h"
#include "..\Common Source\Mesh\Mesh.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\ParticleSystem.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\Particle.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\SceneGraph.h"
#include "..\Common Source\Mesh\Geometry.h"

using namespace DirectX;
using namespace Geometry;

namespace RenderingEngine
{
	Model* FractalTreeRenderer::CreateStandardParticleSystemModel(int maxConcurrentParticleCount) const
	{
		Model* newModel = new Model(this->CreateStandardBufferParticleSystemData(maxConcurrentParticleCount), this->CreateStandardParticleShaderData(SimpleVertexShader, ParticlePixelShader, DefaultInputLayout));
		
		newModel->modelShaderData.primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		newModel->modelShaderData.pixelShader = this->GetPixelShader(ParticlePixelShader);
		newModel->modelShaderData.vertexShader = this->GetVertexShader(SimpleVertexShader);
		newModel->modelShaderData.inputLayout = this->GetInputLayout(DefaultInputLayout);
		newModel->modelShaderData.resourceViewSlot = 1;
		newModel->modelShaderData.samplerStateSlot = 1;
		newModel->modelShaderData.textureResourceView = shaderResourceViews[ExplosionResourceView];
		return newModel;
	}

	void FractalTreeRenderer::UpdateBufferDataFromMesh(ModelBufferData* bufferData, const LSystemMesh* mesh) const
	{
		if(mesh->GetVertexCount() == 0)
		{
			return;
		}

		const unsigned int indCount = mesh->GetIndexCount();
		const unsigned int vCount = mesh->GetVertexCount();

		if (vCount > bufferData->maxVertexCount)
		{
			bufferData->ReleaseVertexBuffers();
			if (mesh->IsLineMesh())
			{
				this->CreateStandardVertexBuffer<VertexColor>(bufferData, vCount * 1.5);
			}
			else
			{
				this->CreateStandardVertexBuffer<TexturedVertex>(bufferData, indCount * 1.5);
			}
		}
		if (indCount > bufferData->maxIndexCount)
		{
			const int newIndCount = indCount * 1.5;
			bufferData->ReleaseIndexBuffers();
			this->CreateStandardIndexBuffers(bufferData, &newIndCount, 1);
		}

		if(mesh->IsLineMesh())
		{
			bufferData->vertexCount = mesh->requiredVertexCount;
			VertexColor* const vertices = new VertexColor[bufferData->vertexCount];

			const unsigned int triangleCount = indCount / 2;

			for(unsigned int a = 0; a < vCount; ++a)
			{
				vertices[a] = {mesh->GetVertices()[a],
					((a / 2) % 2 == 0) ? XMVectorSet(1, 0, 0, 1) : XMVectorSet(0, 1, 0, 1) };
			}

			bufferData->indexCount[0] = indCount;
			shaderindextype* indices = new shaderindextype[bufferData->indexCount[0]];

			for(unsigned int whichLine = 0; whichLine < triangleCount; ++whichLine)
			{
				indices[whichLine*2] = mesh->GetIndices()[whichLine*2];
				indices[whichLine*2 + 1] = mesh->GetIndices()[whichLine*2 + 1];
			}

			D3D11_MAPPED_SUBRESOURCE ms2;
			deviceContext->Map(bufferData->indexBuffer[0], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms2);
			memcpy(ms2.pData, indices, sizeof(shaderindextype) * bufferData->indexCount[0]);
			deviceContext->Unmap(bufferData->indexBuffer[0], NULL);
			delete[] indices;

			D3D11_MAPPED_SUBRESOURCE ms;
			deviceContext->Map(bufferData->vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
			memcpy(ms.pData, vertices, sizeof(VertexColor) * bufferData->vertexCount);
			deviceContext->Unmap(bufferData->vertexBuffer, NULL);
			delete[] vertices;
		}
		else
		{
			const int additionalVertexCount = mesh->requiredVertexCount - mesh->GetVertexCount();
			bufferData->vertexCount = mesh->requiredVertexCount;
			TexturedVertex* const vertices = new TexturedVertex[bufferData->vertexCount];

			for (unsigned int a = 0; a < vCount; ++a)
			{
				vertices[a] = { mesh->vertex[a],
					mesh->normals[a],
					mesh->tangent[a],
					mesh->binormal[a],
					mesh->texCoord[a] };
			}

			const unsigned int triangleCount = indCount / 3;
			bufferData->indexCount[0] = indCount;
			shaderindextype* const indices = new shaderindextype[bufferData->indexCount[0]];

			for(unsigned int whichTriangle = 0; whichTriangle < triangleCount; ++whichTriangle)
			{
				if(mesh->UseFaceNormals())
				{
					const shaderindextype newIndex1 = vCount + whichTriangle*3;
					const shaderindextype newIndex2 = vCount + whichTriangle*3 + 1;
					const shaderindextype newIndex3 = vCount + whichTriangle*3 + 2;
					const TexturedVertex tt = {mesh->vertex[mesh->indices[whichTriangle * 3]],
						mesh->faceNormals[whichTriangle],
						mesh->faceTangents[whichTriangle],
						mesh->faceNormals[whichTriangle],
						mesh->texCoord[mesh->indices[whichTriangle * 3]]};

					const TexturedVertex tt1 = {mesh->vertex[mesh->indices[whichTriangle * 3 + 1]],
						mesh->faceNormals[whichTriangle],
						mesh->faceTangents[whichTriangle],
						mesh->faceNormals[whichTriangle],
						mesh->texCoord[mesh->indices[whichTriangle * 3 + 1]]};

					const TexturedVertex tt2 = {mesh->vertex[mesh->indices[whichTriangle * 3 + 2]],
						mesh->faceNormals[whichTriangle],
						mesh->faceTangents[whichTriangle],
						mesh->faceNormals[whichTriangle],
						mesh->texCoord[mesh->indices[whichTriangle * 3 + 2]]};

					vertices[newIndex1] = tt;
					vertices[newIndex2] = tt1;
					vertices[newIndex3] = tt2;

					indices[whichTriangle*3] = newIndex1;
					indices[whichTriangle*3 + 1] = newIndex2;
					indices[whichTriangle*3 + 2] = newIndex3;
				}
				else
				{
					indices[whichTriangle * 3] = mesh->indices[whichTriangle * 3];
					indices[whichTriangle * 3 + 1] = mesh->indices[whichTriangle * 3 + 1];
					indices[whichTriangle * 3 + 2] = mesh->indices[whichTriangle * 3 + 2];
				}
			}

			D3D11_MAPPED_SUBRESOURCE ms2;
			deviceContext->Map(bufferData->indexBuffer[0], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms2);
			memcpy(ms2.pData, indices, sizeof(shaderindextype) * bufferData->indexCount[0]);
			deviceContext->Unmap(bufferData->indexBuffer[0], NULL);
			delete[] indices;

			D3D11_MAPPED_SUBRESOURCE ms;
			deviceContext->Map(bufferData->vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
			memcpy(ms.pData, vertices, sizeof(TexturedVertex) * bufferData->vertexCount);
			deviceContext->Unmap(bufferData->vertexBuffer, NULL);
			delete[] vertices;
		}
	}

	ModelBufferData* FractalTreeRenderer::CreateStandardBufferParticleSystemData(int maxConcurrentParticleCount) const
	{
		ModelBufferData* newBufferData = new ModelBufferData();

		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(VertexPositionColor) * maxConcurrentParticleCount*4;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		if(FAILED(device->CreateBuffer(&bufferDesc, NULL, &newBufferData->vertexBuffer)))
		{
			exit(20);
		}

		D3D11_BUFFER_DESC indexBufferDesc;
		newBufferData->indexFormat = DEFAULT_INDEX_FORMAT;
		indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		indexBufferDesc.ByteWidth = sizeof(shaderindextype) * maxConcurrentParticleCount*6;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		indexBufferDesc.MiscFlags = 0;
		if(FAILED(device->CreateBuffer(&indexBufferDesc, NULL, &newBufferData->indexBuffer[0])))
		{
			exit(20);
		}

		newBufferData->vertexSize = sizeof(VertexPositionColor);
		newBufferData->indexOffset = 0;
		newBufferData->vertexOffset = 0;
		newBufferData->indexCount[0] = 0;
		newBufferData->vertexCount = 0;
		return newBufferData;
	}

	void FractalTreeRenderer::UpdateBufferFromParticleSystem(ModelBufferData* bufferData, ParticleSystem* particleSystem) const
	{
		if(particleSystem->currentParticleCount <= 0 && bufferData->vertexCount == 0)
		{
			return;
		}

		bufferData->vertexCount = particleSystem->currentParticleCount * 4;
		bufferData->indexCount[0] = particleSystem->currentParticleCount * 6;

		VertexPositionColor* vertices = new VertexPositionColor[bufferData->vertexCount];
		shaderindextype* indices = new shaderindextype[bufferData->indexCount[0]];
	
		//Create four vertices for quad particles.
		for(unsigned int a = 0; a < particleSystem->currentParticleCount; ++a)
		{
			if(!particleSystem->particles[a].active)
			{
				continue;
			}

			XMFLOAT4 v, r, u, p;
			XMFLOAT4 cameraOrientation;
			XMStoreFloat4(&cameraOrientation, this->sceneGraph->GetCameraOrientation());
			XMVECTOR cameraUpVec = XMVector3Transform(XMVectorSet(0, 1, 0, 0), XMMatrixRotationX(cameraOrientation.x));
			cameraUpVec = XMVector3Transform(cameraUpVec, XMMatrixRotationY(cameraOrientation.y));
			const XMVECTOR viewVec = XMVector3Normalize(XMVectorSubtract(this->sceneGraph->GetCameraLocation(), particleSystem->particles[a].location));
			const XMVECTOR rightVec = XMVector3Cross(cameraOrientation.y < 0 ? -cameraUpVec : -cameraUpVec/*this->sceneGraph->GetCameraUp()*/, viewVec);
			//const XMVECTOR rightVec = XMVector3Cross(-this->sceneGraph->GetCameraUp(), viewVec);
			const XMVECTOR upVec = XMVector3Cross(viewVec, rightVec);
				
			XMStoreFloat4(&v, viewVec);
			XMStoreFloat4(&r, rightVec);
			XMStoreFloat4(&u, upVec);
			XMStoreFloat4(&p, particleSystem->particles[a].location);

			const XMMATRIX rotMat = XMMatrixSet(r.x, u.x, v.x, p.x,
												r.y, u.y, v.y, p.y,
												r.z, u.z, v.z, p.z,
												0  , 0,   0,	1);

			const float radius = particleSystem->particles[a].radius;
			VertexPositionColor newVertex[4] = {{XMVectorSubtract(particleSystem->particles[a].location, XMVector3Transform(XMVectorSet(-radius, -radius, 0, 0), rotMat))},
			{XMVectorSubtract(particleSystem->particles[a].location, XMVector3Transform(XMVectorSet(radius, -radius, 0, 0), rotMat))},
			{XMVectorSubtract(particleSystem->particles[a].location, XMVector3Transform(XMVectorSet(radius, radius, 0, 0), rotMat))},
			{XMVectorSubtract(particleSystem->particles[a].location, XMVector3Transform(XMVectorSet(-radius, radius, 0, 0), rotMat))}};

			newVertex[0].texCoord = XMFLOAT2(0, 0);
			newVertex[1].texCoord = XMFLOAT2(1, 0);
			newVertex[2].texCoord = XMFLOAT2(1, 1);
			newVertex[3].texCoord = XMFLOAT2(0, 1);

			for(unsigned int whichVertex = 0; whichVertex < 4; ++whichVertex)
			{
				newVertex[whichVertex].emissiveColor = XMVectorSet(0, 0, 0, 0);
				newVertex[whichVertex].ambientColor = XMVectorSet(0, 0, 0, 0);
				newVertex[whichVertex].specularColor = XMVectorSet(.1f, .1f, .1f, 1);
				newVertex[whichVertex].color = particleSystem->particles[a].tint;
				newVertex[whichVertex].normal = XMVectorSet(0, 0, 1, 0);
				newVertex[whichVertex].tangent = XMVectorSet(0, 1, 0, 0);
				newVertex[whichVertex].binormal = XMVectorSet(1, 0, 0, 0);
				newVertex[whichVertex].specularExponent = 30;
				vertices[a*4 + whichVertex] = newVertex[whichVertex];
			}

			indices[a*6] = a*4;
			indices[a*6 + 1] = a*4 + 1;
			indices[a*6 + 2] = a*4 + 2;

			indices[a*6 + 3] = a*4;
			indices[a*6 + 4] = a*4 + 2;
			indices[a*6 + 5] = a*4 + 3;
		}
		

		D3D11_MAPPED_SUBRESOURCE ms2;
		deviceContext->Map(bufferData->indexBuffer[0], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms2);
		memcpy(ms2.pData, indices, sizeof(shaderindextype) * bufferData->indexCount[0]);
		deviceContext->Unmap(bufferData->indexBuffer[0], NULL);
		delete[] indices;


		D3D11_MAPPED_SUBRESOURCE ms;
		deviceContext->Map(bufferData->vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, vertices, sizeof(VertexPositionColor) * bufferData->vertexCount);
		deviceContext->Unmap(bufferData->vertexBuffer, NULL);
		delete[] vertices;
	}
	
	InstanceBufferData* FractalTreeRenderer::CreateInstanceBufferData(unsigned int instanceCount) const
	{
		InstanceBufferData* instanceBufferData = new InstanceBufferData();
		instanceBufferData->instanceCount = 0;	

		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(InstanceVertex) * instanceCount;
		
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		if(FAILED(device->CreateBuffer(&bufferDesc, NULL, &instanceBufferData->instanceBuffer)))
		{
			assert(false);
			exit(20);
		}

		instanceBufferData->instanceSize = sizeof(InstanceVertex);
		instanceBufferData->instanceOffset = 0;
		return instanceBufferData;
	}

	void FractalTreeRenderer::UpdateInstanceBufferData(InstanceBufferData* instanceBufferdata, XMMATRIX* transforms) const
	{
		D3D11_MAPPED_SUBRESOURCE ms2;
		deviceContext->Map(instanceBufferdata->instanceBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms2);
		memcpy(ms2.pData, transforms, sizeof(XMMATRIX) * instanceBufferdata->instanceCount);
		deviceContext->Unmap(instanceBufferdata->instanceBuffer, NULL);
	}
}