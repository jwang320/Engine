#include "pch.h"
#include "Geometry.h"
#include <vector>
#include "UIRenderer.h"
#include <sstream>
#include <vector>
#include "..\Common Source\LSystem\SpatialLSystemSegment.h"
#include "..\Common Source\LSystem\SystemParameters.h"

using namespace DirectX;
extern double performanceFrequency;

namespace Geometry
{
	const float MathPI = 2*3.14159265359f;

	LSystemMesh::LSystemMesh(const std::vector<LSystemSegment*>& segments,
						     const unsigned int triangleCount,
							 const unsigned int rootSegmentCount,
							 const bool lineMesh,
							 const bool useFaceNormals) :
		segmentCount(segments.size()),
		triangleCount(triangleCount),
		maxVertexCount((segmentCount + 1 + rootSegmentCount) * triangleCount),
		maxIndexCount(maxVertexCount * 6),
		maxFaceCount(segmentCount * triangleCount * 2),
		useFaceNormal(useFaceNormals),
		lineMesh(lineMesh),
		computeNormals(!lineMesh),
		angleIncrement(MathPI / triangleCount),
		vertex(new XMVECTOR[maxVertexCount]),
		normals(new XMVECTOR[maxVertexCount]),
		tangent(new XMVECTOR[maxVertexCount]),
		binormal(new XMVECTOR[maxVertexCount]),
		texCoord(new XMFLOAT2[maxVertexCount]),
		faceNormals(new DirectX::XMVECTOR[maxFaceCount]),
		faceTangents(new DirectX::XMVECTOR[maxFaceCount]),
		indices(new unsigned int[maxIndexCount]),
		geometrySegments(new GeometrySegment[segmentCount]),
		quadIndices(new QuadFace[(segmentCount) * this->triangleCount])
	{
		if (!this->lineMesh)
		{
			for (unsigned int a = 0; a < segments.size(); ++a)
			{
				this->AddCylindarSegmentSkeleton(static_cast<const SpatialLSystemSegment*>(segments[a]), a);
			}

			this->ComputeNormals();
		}
		else
		{
			for (unsigned int a = 0; a < segments.size(); ++a)
			{
				const SpatialLSystemSegment* const segment = (const SpatialLSystemSegment* const)segments[a];
				this->AddLineVertex(segment->GetStartPoint());
				this->AddLineVertex(segment->GetEndPoint());
				this->AddFace2(a << 1, (a << 1) + 1);
			}
		}
	}

	//Add a cylindar segment using the interpolation method for curve continuity.
	//It's too late to fix the spelling of cylindar.  It's spread everywhere.
	void LSystemMesh::AddCylindarSegmentSkeleton(const SpatialLSystemSegment* segment, const int segmentIndex)
	{
		const int parentSegmentIndex = !segment->IsRoot() ? segment->GetParent()->GetLocalSegmentID() : -1;

		const XMVECTOR axis = XMVector3Normalize(XMVectorSubtract(segment->GetEndPoint(), segment->GetStartPoint()));

		if(segment->IsRoot()) //Add a base segment to start the mesh.
		{
			const XMVECTOR offset = XMVector3Length(XMVectorSubtract(segment->GetStartPoint(), segment->GetEndPoint()));
			for(unsigned int whichTriangle = 0; whichTriangle < this->triangleCount; ++whichTriangle)
			{
				this->geometrySegments[segmentIndex].indices[whichTriangle] = this->vertexCount;

				const float angle = this->angleIncrement * whichTriangle;
				const float varianceAmount = segmentIndex == 0 ? SystemParameters::TrunkBaseVarianceFactorFunction(segment, whichTriangle) : 1.f;
				
				XMVECTOR vert = XMVectorSet(cosf(angle + segment->GetTotalYRotation()) * segment->GetStartRadius() * varianceAmount,
					0, sinf(angle + segment->GetTotalYRotation()) * segment->GetStartRadius() * varianceAmount, 0);
				vert = XMVector3Transform(vert, segment->GetEndTransformation());//XMVectorAdd(vert, segment->GetStartPoint());
				vert = XMVectorSubtract(vert, XMVectorMultiply(axis, offset));
				this->AddVertex(vert, XMFLOAT2((whichTriangle % 2 == 0) ? 0 : 1.f, 0.f));
			}
		}

		this->geometrySegments[segmentIndex].axis = axis;
		this->geometrySegments[segmentIndex].center = segment->GetEndPoint();

		XMFLOAT4 segmentConnectionAngle = XMFLOAT4(0, 0, 0, 0);
		XMFLOAT4 segmentRotationOffset = XMFLOAT4(0, 0, 0, 0);
		float maxLengthAdd = 0;

		if(parentSegmentIndex > -1)
		{
			const int childCount = segment->GetParent()->GetNeighbors().size();

			const XMVECTOR parentAxis = this->geometrySegments[parentSegmentIndex].axis;

			//Check to see if the previous segment is at a different angle relative
			//to the new segment.
			XMFLOAT4 newDot;
			XMStoreFloat4(&newDot, XMVector3Dot(axis, parentAxis));
			
			if (abs(newDot.x) < .999999f && childCount < 2) //this doesn't exactly work because floating point.
			{
				const XMVECTOR crossProduct = XMVector3Normalize(XMVector3Cross(parentAxis, axis));
				XMStoreFloat4(&segmentConnectionAngle, XMVector3AngleBetweenNormals(axis, parentAxis));

				const float rotAngle = segmentConnectionAngle.x * .5f + 3.14159265359f * .5f;

				//Create a bisecting line between the parent segment's axis and the new segment's axis
				const XMVECTOR bisector = XMVector3Transform(this->geometrySegments[segmentIndex].axis, 
					XMMatrixRotationAxis(crossProduct, -rotAngle));

				const XMVECTOR planeNormal =XMVector3Cross(crossProduct, parentAxis);
				const XMVECTOR previousStartVertex = this->vertex[this->geometrySegments[parentSegmentIndex].indices[0]];
				const XMVECTOR prevNorm = XMVector3Normalize(XMVectorSubtract(previousStartVertex,
					segment->GetStartPoint()));

				//Use segmentRotationOffset to make sure the circle of vertices is built starting where the cosine is 1.
				XMStoreFloat4(&segmentRotationOffset, XMVector3AngleBetweenVectors(planeNormal, prevNorm));
				XMStoreFloat4(&segmentConnectionAngle, XMVector3AngleBetweenNormals(bisector, planeNormal));

				XMFLOAT4 crossProduct3F;
				XMStoreFloat4(&crossProduct3F, XMVector3Cross(bisector, planeNormal));

				//If the cross product of the two axises is negative,
				//the angle is in the other quadrant.
				if(crossProduct3F.z > 0) 
				{
					segmentConnectionAngle.x = -segmentConnectionAngle.x;
				}

				float totalY = segment->GetTotalYRotation();
				while(totalY > 3.14159265359f * 2)
				{
					totalY-= 3.14159265359f * 2; 
				}

				if(totalY > 3.14159265359f)
				{
					segmentRotationOffset.x = -segmentRotationOffset.x;
				}

				const float lengthRatio = tanf(segmentConnectionAngle.x);
				maxLengthAdd =  lengthRatio * segment->GetStartRadius();
			}
		}

		//Create a circle of vertices at the end of the segment perpendicular
		//to the axis of the mesh's skeleton.
		for(unsigned int whichTriangle = 0; whichTriangle < this->triangleCount; ++whichTriangle)
		{
			this->geometrySegments[segmentIndex].indices[whichTriangle] = this->vertexCount;
			this->geometrySegments[segmentIndex].vertexConnections[whichTriangle] = whichTriangle;
			
			float angle = this->angleIncrement * whichTriangle;
			const float varianceAmount = !SystemParameters::UseTrunkBodyVariance ? 1.f : clamp((rand()%SystemParameters::TrunkBaseRadiusVariance)*.01f + 1, SystemParameters::TrunkBaseMinimumRadiusVariance, SystemParameters::TrunkBaseMaximumRadiusVariance);
			
			XMVECTOR vert = XMVectorSet(cosf(angle + segment->GetTotalYRotation()) * segment->GetEndRadius() * varianceAmount,
				0,
				sinf(angle + segment->GetTotalYRotation()) * segment->GetEndRadius() * varianceAmount,
				0);

			vert = XMVector3Transform(vert, segment->GetEndTransformation());

			if(parentSegmentIndex > -1)
			{
				//If the segment is curved, then modify the vertices of the previous segment
				//so that the segment connections preserve derivative continuity.
				if(maxLengthAdd != 0)
				{
					angle-= segmentRotationOffset.x;
					XMVECTOR& oldVert = this->vertex[this->geometrySegments[parentSegmentIndex].indices[whichTriangle]];
					
					const float lenVal = fabs(maxLengthAdd) * cosf(angle);
					const XMVECTOR offSet = XMVectorMultiply(this->geometrySegments[parentSegmentIndex].axis, XMVectorReplicate(-lenVal));

					oldVert = XMVectorAdd(oldVert, offSet);			
				}

				this->AddVertex(vert, XMFLOAT2((whichTriangle % 2 == 0) ? 0.f : 1.f,
					this->texCoord[this->geometrySegments[parentSegmentIndex].indices[whichTriangle]].y == 0 ? 1.f : 0.f));
			}
			else
			{
				this->AddVertex(vert, XMFLOAT2((whichTriangle % 2 == 0) ? 0.f : 1.f, 1.f));
			}		
		}

		this->AddCylinderFaces(parentSegmentIndex, segmentIndex);
	}

	void LSystemMesh::AddCylinderFaces(const short parentSegmentIndex, const unsigned short segmentIndex)
	{
		const int whichFace = this->indexCount / 3;

		const unsigned int start = this->vertexCount - this->triangleCount*2;
		unsigned int quadIndex = 0;
		for(unsigned int whichTriangle = 0; whichTriangle < this->triangleCount; ++whichTriangle)
		{
			const unsigned int ind1 = parentSegmentIndex > -1 ? this->geometrySegments[parentSegmentIndex].indices[this->geometrySegments[segmentIndex].vertexConnections[whichTriangle] ] : 
				start + whichTriangle;
			const unsigned int ind2 = this->geometrySegments[segmentIndex].indices[whichTriangle];
			unsigned int ind3;
			unsigned int ind4;
			if(whichTriangle < this->triangleCount - 1)
			{
				ind3 = this->geometrySegments[segmentIndex].indices[whichTriangle + 1];
				ind4 = parentSegmentIndex > -1 ? this->geometrySegments[parentSegmentIndex].indices[this->geometrySegments[segmentIndex].vertexConnections[whichTriangle + 1]] :
					start + whichTriangle + 1;
			}
			else
			{
				ind3 = this->geometrySegments[segmentIndex].indices[0];
				ind4 = parentSegmentIndex > -1 ? this->geometrySegments[parentSegmentIndex].indices[this->geometrySegments[segmentIndex].vertexConnections[0]] :
					start + 0;
			}

			this->AddFace(ind1, ind2, ind3);
			this->AddFace(ind3, ind4, ind1);
			this->geometrySegments[segmentIndex].quadIndices[whichTriangle] = whichTriangle;
			this->quadIndices[segmentIndex * this->triangleCount + whichTriangle] = {{ind1, ind2, ind3, ind4}, {whichFace + whichTriangle * 2, whichFace + whichTriangle * 2 + 1}};
			++quadIndex;
		}
	}

	void LSystemMesh::ComputeNormals()
	{
		int segmentsComputed = 0;
		for(unsigned int whichTriangle = 0; whichTriangle < this->vertexCount; ++whichTriangle)
		{
			this->normals[whichTriangle] = XMVectorSet(0,0,0,0);
			this->tangent[whichTriangle] = XMVectorSet(0,0,0,0);
			this->binormal[whichTriangle] = XMVectorSet(0,0,0,0);
			++segmentsComputed;
		}

		const unsigned int quadIndexCount = this->triangleCount * this->segmentCount;
		for(unsigned int whichQuad = 0; whichQuad < quadIndexCount; ++whichQuad)
		{
			if (this->quadIndices[whichQuad].triangleIndices[0] != 200000000 && this->quadIndices[whichQuad].triangleIndices[1] != 200000000)
			{
				const unsigned int ind1 = this->quadIndices[whichQuad].indices[0];
				const unsigned int ind2 = this->quadIndices[whichQuad].indices[1];
				const unsigned int ind3 = this->quadIndices[whichQuad].indices[2];
				const unsigned int ind4 = this->quadIndices[whichQuad].indices[3];
				const unsigned int triangleInd1 = this->quadIndices[whichQuad].triangleIndices[0];
				const unsigned int triangleInd2 = this->quadIndices[whichQuad].triangleIndices[1];

				const XMVECTOR averageNormal = XMVectorMultiply(XMVectorAdd(this->faceNormals[triangleInd1], this->faceNormals[triangleInd2]), XMVectorReplicate(.5f));
				const XMVECTOR averageTangent = XMVectorMultiply(XMVectorAdd(this->faceTangents[triangleInd1], this->faceTangents[triangleInd2]), XMVectorReplicate(.5f));

				this->normals[ind1] = XMVectorAdd(
					this->normals[ind1],
					this->faceNormals[triangleInd1]);
						
				this->tangent[ind1] = XMVectorAdd(
					this->tangent[ind1],
					this->faceTangents[triangleInd1]);

				this->normals[ind2] = XMVectorAdd(
					this->normals[ind2],
					averageNormal);

				this->tangent[ind2] = XMVectorAdd(
					this->tangent[ind2],
					averageTangent);

				this->normals[ind3] = XMVectorAdd(
					this->normals[ind3],
					this->faceNormals[triangleInd2]);

				this->tangent[ind3] = XMVectorAdd(
					this->tangent[ind3],
					this->faceTangents[triangleInd2]);

				this->normals[ind4] = XMVectorAdd(
					this->normals[ind4],
					averageNormal);

				this->tangent[ind4] = XMVectorAdd(
					this->tangent[ind4],
					averageTangent);
			}
		}

		for (unsigned int whichVertex = 0; whichVertex < this->vertexCount; ++ whichVertex)
		{
			this->normals[whichVertex] = XMVector3Normalize(this->normals[whichVertex]);
			this->tangent[whichVertex] = XMVector3Normalize(this->tangent[whichVertex]);
			this->binormal[whichVertex] = XMVector3Normalize(XMVector3Cross(this->normals[whichVertex], this->tangent[whichVertex]));
		}
	}

	void LSystemMesh::AddVertex(const XMVECTOR& v, const XMFLOAT2& texCoord)
	{
		this->vertex[this->vertexCount] = v;
		this->texCoord[this->vertexCount] = texCoord;
		++this->requiredVertexCount;
		++this->vertexCount;
	}

	void LSystemMesh::AddLineVertex(const DirectX::XMVECTOR& v)
	{
		this->vertex[this->vertexCount] = v;
		++this->requiredVertexCount;
		++this->vertexCount;
	}

	//For adding a line face.  Line and triangle faces should not be mixed in the same mesh!
	void LSystemMesh::AddFace2(const unsigned int ind1, const unsigned int ind2)
	{
		this->indices[this->indexCount] = ind1;
		++this->indexCount;
		this->indices[this->indexCount] = ind2;
		++this->indexCount;
	}

	//For adding a triangle face.  Line and triangle faces should not be mixed in the same mesh!
	void LSystemMesh::AddFace(const unsigned int ind1, const unsigned int ind2, const unsigned int ind3)
	{
		this->indices[this->indexCount] = ind1;
		++this->indexCount;
		this->indices[this->indexCount] = ind2;
		++this->indexCount;
		this->indices[this->indexCount] = ind3;
		++this->indexCount;

		const XMVECTOR edge1 = XMVectorSubtract(this->vertex[ind2], this->vertex[ind1]);
		const XMVECTOR edge2 = XMVectorSubtract(this->vertex[ind3], this->vertex[ind1]);
		const XMVECTOR normal = XMVector3Normalize(XMVector3Cross(edge1, edge2));
		const XMFLOAT2 edge1UV = XMFLOAT2(this->texCoord[ind2].x - this->texCoord[ind1].x, this->texCoord[ind2].y - this->texCoord[ind1].y);
		const XMFLOAT2 edge2UV = XMFLOAT2(this->texCoord[ind3].x - this->texCoord[ind1].x, this->texCoord[ind3].y - this->texCoord[ind1].y);

		const float cp = edge1UV.y * edge2UV.x - edge1UV.x * edge2UV.y;
		if (cp != 0.0f)
		{
			const float mul = 1.0f / cp;
			const XMVECTOR tangent = XMVector3Normalize(XMVectorMultiply(XMVectorAdd(XMVectorMultiply(edge1, XMVectorReplicate(-edge2UV.y)), XMVectorMultiply(edge2, XMVectorReplicate(-edge1UV.y))), XMVectorReplicate(mul)));

			this->faceTangents[this->faceCount] = tangent;
		}
		else
		{
			this->faceTangents[this->faceCount] = normal;
		}

		this->faceNormals[this->faceCount] = normal;
		if (this->useFaceNormal)
		{
			this->requiredVertexCount += 3;
		}

		++this->faceCount;
	}

	LSystemMesh::~LSystemMesh()
	{	
		delete[] this->geometrySegments;
		delete[] this->quadIndices;
		delete[] this->vertex;
		delete[] this->normals;
		delete[] this->binormal;
		delete[] this->tangent;
		delete[] this->texCoord;
		delete[] this->indices;
		delete[] this->faceNormals;
		delete[] this->faceTangents;
	}
}
