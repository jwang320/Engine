#pragma once
#include "..\Common Source\Mesh\Mesh.h"

class LSystem;
class ConnectorTrunkSegment;
class SpatialLSystemSegment;
class LSystemSegment;

namespace Geometry
{
	struct QuadFace
	{
		unsigned int indices[4];
		unsigned int triangleIndices[2];
	};

	inline float clamp(float val, float minVal, float maxVal)
	{
		return val < minVal ? minVal : (val > maxVal ? maxVal : val);
	}

	inline float randomClamp(float val, float minVal, float maxVal)
	{
		if (val < minVal || val > maxVal)
		{
			if (rand() % 2 == 0)
				return minVal;
			return maxVal;
		}

		return val;
	}

	//The GeometrySegment corresponds to a single segment of the LSystem, and maps
	//vertices in the mesh to the segment.  Each lsystem segment has a single geometry segment
	//mapped to it (with a circle of vertices), except for the root segment, which must have 2 
	//circles of vertices.
	struct GeometrySegment
	{
		//The list of indices of vertices in the Mesh's full vertex list which creates
		//the circle of vertices around the top center point of the LSystemSegment line.
		unsigned int indices[8];
		unsigned int quadIndices[8];
		DirectX::XMVECTOR axis;
		DirectX::XMVECTOR center;
		//The boundSegmentLocalID is the ID number of the LSystemSegment object that this
		//geometry segment is mapped to.
		//The vertexConnections list is a list of indices parallel to the indices list.  It designates
		//which existing vertex the newly added vertex connects to.
		unsigned int vertexConnections[8];
	};

	class LSystemMesh
	{
	private:
		const unsigned int segmentCount;
		const unsigned int triangleCount;

		const unsigned int maxVertexCount;
		const unsigned int maxIndexCount;
		const unsigned int maxFaceCount;

		const bool useFaceNormal;
		const bool lineMesh;
		const bool computeNormals;
		const float angleIncrement;

		unsigned int faceCount = 0;
		unsigned int vertexCount = 0;
		unsigned int indexCount = 0;

	public:
		DirectX::XMVECTOR* const vertex;
		DirectX::XMVECTOR* const normals;
		DirectX::XMVECTOR* const tangent;
		DirectX::XMVECTOR* const binormal;
		DirectX::XMFLOAT2* const texCoord;

		DirectX::XMVECTOR* const faceNormals;
		DirectX::XMVECTOR* const faceTangents;
		unsigned int* const indices;

		GeometrySegment* const geometrySegments;
		QuadFace* const quadIndices;

		int requiredVertexCount = 0;

	public:
		//this constructor will initialize the mesh.
		LSystemMesh(const std::vector<LSystemSegment*>& segments, const unsigned int triangleCount, const unsigned int rootSegmentCount, const bool lineMesh, const bool useFaceNormals);
		~LSystemMesh();

	public:
		const DirectX::XMVECTOR* GetVertices() const { return this->vertex; }
		const DirectX::XMVECTOR* GetNormals() const { return this->normals; }
		const DirectX::XMVECTOR* GetBinormals() const { return this->binormal; }
		const DirectX::XMVECTOR* GetTangents() const { return this->tangent; }
		const DirectX::XMFLOAT2* GetTexCoords() const { return this->texCoord; }
		const unsigned int* GetIndices() const { return this->indices; }
		unsigned int GetIndexCount() const { return this->indexCount; }
		unsigned int GetVertexCount() const { return this->vertexCount; }
		unsigned int GetFaceCount() const { return this->faceCount; }
		bool UseFaceNormals() const { return this->useFaceNormal; }
		bool IsLineMesh() const { return this->lineMesh; }

	private:
		//AddCylindarSegment_____ are the functions that create the circle of vertices around each segment and add them to the mesh list
		void AddCylindarSegmentSkeleton(const SpatialLSystemSegment* segment, const int segmentIndex);
		void AddVertex(const DirectX::XMVECTOR& v, const DirectX::XMFLOAT2& texCoord);
		void AddLineVertex(const DirectX::XMVECTOR& v);
		void AddFace2(unsigned int ind1, unsigned int ind2);
		void AddFace(unsigned int ind1, unsigned int ind2, unsigned int ind3);

		//AddCylinderFaces is called after AddCylindarSegment____ and is meant to create the face list for use
		//in the Mesh object.
		void AddCylinderFaces(const short parentSegmentIndex, const unsigned short segmentIndex);
		void ComputeNormals();
	};
}