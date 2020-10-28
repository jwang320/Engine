#include "pch.h"
#include "HeightMapMesh.h"
#include <stdlib.h>

using namespace DirectX;
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
namespace Geometry
{
	UniformHeightMapMesh::UniformHeightMapMesh(int width, int length, float gridWidth, float xCenter, float zCenter, bool useLod, int terrainTextureRepeatCount, bool alternateCellOrientation) :
		width(width),
		gridWidth(gridWidth), 
		xCenter(xCenter),
		zCenter(zCenter),
		totalWidth(width * gridWidth),
		useLod(useLod),
		terrainTextureRepeatCount(terrainTextureRepeatCount),
		alternateCellOrientation(alternateCellOrientation)
	{
		for(int a = 0; a < LOD_COUNT; ++a)
		{
			this->detailLevel[a] = powf(2, a);
		}

		const int count = this->width * this->width;
		vertCountOff[0] = 0;
		vertCountOff[1] = count;

		for(int a = 2; a < LOD_COUNT; ++a)
		{
			vertCountOff[a] = vertCountOff[a - 1] + (width / detailLevel[a - 1] + 1) * (width / detailLevel[a - 1] + 1);
		}
		if(this->useLod)
		{
			this->lodLevel = LOD_COUNT;
			this->indices[0].reserve(count*6);

			for(int a = 1; a < LOD_COUNT; ++a)
			{
				this->indices[a].reserve((width / detailLevel[a - 1] + 1) * (width / detailLevel[a - 1] + 1) * 6);
			}

			this->vertex.reserve(vertCountOff[LOD_COUNT - 1]*2);
			this->normals.reserve(vertCountOff[LOD_COUNT - 1]*2);
			this->tangent.reserve(vertCountOff[LOD_COUNT - 1]*2);
			this->binormal.reserve(vertCountOff[LOD_COUNT - 1]*2);
			this->texCoord.reserve(vertCountOff[LOD_COUNT - 1]*2);
		}
		else
		{
			this->vertex.reserve(count);
			this->binormal.reserve(count);
			this->normals.reserve(count);
			this->tangent.reserve(count);
			this->texCoord.reserve(count);
			this->indices[0].reserve(count*6);
			this->lodLevel = 1;
		}

		this->lodGridWidth[0] = this->width;
		this->lodGridLength[0] = this->width;

		this->boundingBox.maxBounds = XMVectorSet(this->xCenter + this->totalWidth, 1, this->zCenter + this->totalWidth, 1);
		this->boundingBox.minBounds = XMVectorSet(this->xCenter, -1, this->zCenter, 1);
	}

	XMVECTOR UniformHeightMapMesh::GetNormal(float xLoc, float zLoc, float xDist, float zDist) const
	{
		const float h0 = this->generator->GenerateHeightValue(xLoc, zLoc - zDist);
		const float h1 = this->generator->GenerateHeightValue(xLoc - xDist, zLoc);
		const float h2 = this->generator->GenerateHeightValue(xLoc + xDist, zLoc);
		const float h3 = this->generator->GenerateHeightValue(xLoc, zLoc + zDist);

		const XMVECTOR normal = XMVectorSet(h1 - h2, xDist + zDist, h0 - h3, 0);
		return XMVector3Normalize(normal);
	}

	XMVECTOR UniformHeightMapMesh::GetNormal(float xLoc, float zLoc, float dist) const
	{
		const float h0 = this->generator->GenerateHeightValue(xLoc, zLoc - dist);
		const float h1 = this->generator->GenerateHeightValue(xLoc - dist, zLoc);
		const float h2 = this->generator->GenerateHeightValue(xLoc + dist, zLoc);
		const float h3 = this->generator->GenerateHeightValue(xLoc, zLoc + dist);

		const XMVECTOR normal = XMVectorSet(h1 - h2, 2 * dist, h0 - h3, 0);
		return XMVector3Normalize(normal);
	}

	void UniformHeightMapMesh::SetGenerator(const ProceduralGeneration::PerlinHeightMapGenerator* generator, bool generateMesh)
	{
		this->generator = generator;
		if(generateMesh)
		{
			this->createMultiBufferUniformPolygonMesh();
		}
	}
	
	void UniformHeightMapMesh::createMultiBufferUniformPolygonMesh()
	{
		this->generateBuffer();

		for(int a = 1; a < this->lodLevel; ++a)
		{
			this->generateBuffer(a);
		}
	}

	void UniformHeightMapMesh::generateBuffer(int lodLevel)
	{
		const int wid = (this->width / this->detailLevel[lodLevel]) + (lodLevel > 0 ? 1 : 0);/*(this->detailLevel[lodLevel];)*/
		const float gridWid = (this->gridWidth * this->detailLevel[lodLevel]);//lodLevel > 0 ? this->gridWidth * this->detailLevel[lodLevel] : this->gridWidth;

		float lastZGenerated = 1;
		for(int z = 0; z < wid; ++z)
		{
			//float xGenerationsSkipped; //possibly implement dynamic resolution heightmap in the future
			for(int x = 0; x < wid; ++x)
			{
				const int loc = this->width * z * this->detailLevel[lodLevel] + x * this->detailLevel[lodLevel];
				MeshVertex newVertex;
				newVertex.vertex = this->vertex[loc];
				//newVertex.texCoord = XMFLOAT2(static_cast<float>(x)/wid*FractalTreeResources::terrainTextureRepeatCount, static_cast<float>(z) / wid*FractalTreeResources::terrainTextureRepeatCount);
				newVertex.texCoord = this->texCoord[loc];
				XMFLOAT4 oldVert;
				XMStoreFloat4(&oldVert, newVertex.vertex);
				newVertex.normal = this->GetNormal(oldVert.x, oldVert.z, gridWid);
				newVertex.tangent = XMVector3Normalize(XMVector3Cross(XMVectorSet(1, 0, 0, 0), newVertex.normal));
				newVertex.binormal = XMVector3Cross(newVertex.tangent, newVertex.normal);
				this->AddVertex(newVertex);
			}
		}

		this->createFaces(lodLevel);
	}

	void UniformHeightMapMesh::generateBuffer()
	{
		for(int z = 0; z < this->width; ++z)
		{
			const float zLoc = z * this->gridWidth + this->zCenter;
			for(int x = 0; x < this->width; ++x)
			{
				const float xLoc = x * this->gridWidth + this->xCenter;
				MeshVertex newVertex;
				const float calculatedHeight = this->generator->GenerateHeightValue(xLoc, zLoc);
				newVertex.vertex = XMVectorSet(xLoc, calculatedHeight, zLoc, 0);
				newVertex.texCoord = XMFLOAT2(static_cast<float>(x)/this->width*this->terrainTextureRepeatCount, static_cast<float>(z) / this->width*this->terrainTextureRepeatCount);


				newVertex.normal = this->GetNormal(xLoc, zLoc, this->gridWidth);
				newVertex.tangent = XMVector3Normalize(XMVector3Cross(XMVectorSet(1, 0, 0, 0), newVertex.normal));
				newVertex.binormal = XMVector3Cross(newVertex.tangent, newVertex.normal);
				this->AddVertex(newVertex);
			}
		}

		this->createFaces(0);
	}

	void UniformHeightMapMesh::createFaces(int lodLevel)
	{
		const int detailWidth = (this->width / this->detailLevel[lodLevel]) + (lodLevel > 0 ? 1 : 0);
		for(int z = 0; z < detailWidth - 1; ++z)
		{
			if(z % 2 == 0 || !this->alternateCellOrientation)
			{
				for(int x = 0; x < detailWidth - 1; ++x)
				{	
					const int loc = detailWidth * z + x + this->vertCountOff[lodLevel];
					if(x % 2 == 0 || !this->alternateCellOrientation)
					{
						this->AddFace(loc, loc + detailWidth + 1, loc + 1, false, lodLevel);
						this->AddFace(loc, loc + detailWidth, loc + detailWidth + 1, false, lodLevel);	
					}
					else
					{
						this->AddFace(loc + 1, loc, loc + detailWidth, false, lodLevel);
						this->AddFace(loc + 1, loc + detailWidth, loc + detailWidth + 1, false, lodLevel);	
					}
				}
			}
			else
			{
				for(int x = 0; x < (lodLevel > 0 ? detailWidth - 1: detailWidth - 1); ++x)
				{	
					const int loc = (detailWidth * z + x) + this->vertCountOff[lodLevel];
					if(x % 2 != 0)
					{
						this->AddFace(loc, loc + detailWidth + 1, loc + 1, false, lodLevel);
						this->AddFace(loc, loc + detailWidth, loc + detailWidth + 1, false, lodLevel);	
					}
					else
					{
						this->AddFace(loc + 1, loc, loc + detailWidth, false, lodLevel);
						this->AddFace(loc + 1, loc + detailWidth, loc + detailWidth + 1, false, lodLevel);	
					}
				}
			} 
		}
	}

	void UniformHeightMapMesh::AddFace(unsigned int ind1, unsigned int ind2, unsigned int ind3, bool useFaceNormal, int lodIndex) 
	{
		this->indices[lodIndex].push_back(ind1);
		this->indices[lodIndex].push_back(ind2);
		this->indices[lodIndex].push_back(ind3);
		if(lodIndex == 0)
		{
			this->faceData.useFaceNormals.push_back(useFaceNormal);
			if(useFaceNormal)
			{
				this->faceData.faceNormals.push_back(XMVector3Normalize(
					XMVector3Cross(
					XMVectorSubtract(this->vertex[ind2],this->vertex[ind1]),
					XMVectorSubtract(this->vertex[ind3], this->vertex[ind1]))));

				this->requiredVertexCount += 3;
			}
		}
	}
}