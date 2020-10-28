#include "pch.h"
#include "Mesh.h"

using namespace DirectX;
using namespace std;

namespace Geometry
{
	Mesh::Mesh() : computeNormals(true), requiredVertexCount(0), lodLevel(1), lineMesh(false)
	{
		this->boundingBox.maxBounds = XMVectorSet(-5000000, -5000000, -5000000, 1);
		this->boundingBox.minBounds = XMVectorSet(5000000, 5000000, 5000000, 1);
	}

	void Mesh::AddVertex(const MeshVertex& vertex)
	{
		this->SetVertex(vertex);
		XMFLOAT4 tempVert;
		XMFLOAT4 tempMinBounds;
		XMFLOAT4 tempMaxBounds;
		XMStoreFloat4(&tempVert, vertex.vertex);
		XMStoreFloat4(&tempMinBounds, this->boundingBox.minBounds);
		XMStoreFloat4(&tempMaxBounds, this->boundingBox.maxBounds);
		if(tempVert.x < tempMinBounds.x)
		{
			tempMinBounds.x = tempVert.x;
		}
		else if(tempVert.x  > tempMaxBounds.x)
		{
			tempMaxBounds.x = tempVert.x;
		}
		if(tempVert.y < tempMinBounds.y)
		{
			tempMinBounds.y = tempVert.y;
		}
		else if(tempVert.y > tempMaxBounds.y)
		{
			tempMaxBounds.y = tempVert.y;
		}
		if(tempVert.z < tempMinBounds.z)
		{
			tempMinBounds.z = tempVert.z;
		}
		else if(tempVert.z > tempMaxBounds.z)
		{
			tempMaxBounds.z = tempVert.z;
		}

		this->boundingBox.minBounds = XMLoadFloat4(&tempMinBounds);
		this->boundingBox.maxBounds = XMLoadFloat4(&tempMaxBounds);

		++this->requiredVertexCount;
	}

	//For adding a triangle face.  Line and triangle faces should not be mixed in the same mesh!
	void Mesh::AddFace(unsigned int ind1, unsigned int ind2, unsigned int ind3, bool useFaceNormal, int lodIndex) 
	{
		this->indices[lodIndex].push_back(ind1);
		this->indices[lodIndex].push_back(ind2);
		this->indices[lodIndex].push_back(ind3);
		if(lodIndex == 0)
		{
			this->faceData.useFaceNormals.push_back(useFaceNormal);
			this->faceData.faceNormals.push_back(XMVector3Normalize(
				XMVector3Cross(
				XMVectorSubtract(this->vertex[ind2],this->vertex[ind1]),
				XMVectorSubtract(this->vertex[ind3], this->vertex[ind1]))));

			if(useFaceNormal)
			{
				this->requiredVertexCount += 3;
			}
		}
	}

	void Mesh::AddFace2(unsigned int ind1, unsigned int ind2) 
	{
		this->indices[0].push_back(ind1);
		this->indices[0].push_back(ind2);
	}

	void Mesh::ClearData()
	{
		this->vertex.clear();
		this->vertex.shrink_to_fit();
		this->normals.clear();
		this->normals.shrink_to_fit();
		this->tangent.clear();
		this->tangent.shrink_to_fit();
		this->binormal.clear();
		this->binormal.shrink_to_fit();
		this->texCoord.clear();
		this->texCoord.shrink_to_fit();

		this->faceData.useFaceNormals.clear();
		this->faceData.useFaceNormals.shrink_to_fit();

		this->faceData.faceNormals.clear();
		this->faceData.faceNormals.shrink_to_fit();

		this->faceData.faceTangents.clear();
		this->faceData.faceTangents.shrink_to_fit();

		for(int a = 0; a < this->lodLevel; ++a)
		{
			this->indices[a].clear();
			this->indices[a].shrink_to_fit();
		}
	}

	Mesh::~Mesh()
	{

	}
}
