#pragma once
#include <vector>
#ifndef _STRATEGY_GAME
#ifndef _APP1
#ifndef _RTS_GAME
#include "BoundingBox.h"
#else
#include "..\src\BoundingBox.h"
#endif
#else
#endif
#else
#include "..\src\BoundingBox.h"
#endif
#define LOD_COUNT 6

namespace Geometry
{
	struct MeshVertex
	{
		DirectX::XMVECTOR vertex;
		DirectX::XMVECTOR normal;
		DirectX::XMVECTOR tangent;
		DirectX::XMVECTOR binormal;
		DirectX::XMFLOAT2 texCoord;

		bool vertexNormalChanged;
	};

	struct FaceData
	{
		std::vector<DirectX::XMVECTOR> faceNormals;
		std::vector<DirectX::XMVECTOR> faceTangents;
		std::vector<char> useFaceNormals;
	};

	class Mesh
	{
	public:
		std::vector<DirectX::XMVECTOR> vertex;
		std::vector<DirectX::XMVECTOR> normals;
		std::vector<DirectX::XMVECTOR> tangent;
		std::vector<DirectX::XMVECTOR> binormal;
		std::vector<DirectX::XMFLOAT2> texCoord;

		FaceData faceData;
		std::vector<unsigned int> indices[LOD_COUNT];
		bool computeNormals;
		int requiredVertexCount;
		int lodLevel;
		int detailLevel[LOD_COUNT]; // lower is better
		int lodGridWidth[LOD_COUNT];
		int lodGridLength[LOD_COUNT];
		bool lineMesh;
		BoundingBox boundingBox;

	public:
		Mesh();
		virtual ~Mesh();

	protected:
		void AddFace(unsigned int ind1, unsigned int ind2, unsigned int ind3, bool useFaceNormal = true, int lodIndex = 0);

	public:
		void AddVertex(const MeshVertex& vertex);
		void AddFace2(unsigned int ind1, unsigned int ind2);
		const std::vector<unsigned int>& GetIndices(int bufferIndex = 0) const {return this->indices[bufferIndex];}
		int GetLODLevel() const {return this->lodLevel;}
		int GetVertexCount() const {return static_cast<unsigned int>(this->vertex.size());}
		void SetComputeNormals(bool computeNormals) {this->computeNormals = computeNormals;}
		bool IsComputeNormals() const {return this->computeNormals;}
		int GetDetailLevel(int index) const {return this->detailLevel[index];}
		int GetLODGridWidth(int index) const {return this->lodGridWidth[index];}
		int GetLODGridLength(int index) const {return this->lodGridLength[index];}
		void SetVertex(const MeshVertex& v) 
		{
			//this->vertex.push_back(v);
			this->vertex.push_back(v.vertex);
			if (this->computeNormals)
			{
				this->normals.push_back(v.normal);
				this->tangent.push_back(v.tangent);
				this->binormal.push_back(v.binormal);
				this->texCoord.push_back(v.texCoord);
			}
		}//{this->vertices[this->GetVertexCount()] = v; ++this->vertexCount;}
		void ClearData();
		const std::vector<DirectX::XMVECTOR>& GetVertices() const {return this->vertex;}
		const std::vector<DirectX::XMVECTOR>& GetNormals() const {return this->normals;}
		const std::vector<DirectX::XMVECTOR>& GetBinormals() const {return this->binormal;}
		const std::vector<DirectX::XMVECTOR>& GetTangents() const {return this->tangent;}
		const std::vector<DirectX::XMFLOAT2>& GetTexCoords() const {return this->texCoord;}
	};
}