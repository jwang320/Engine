#pragma once
#include <sstream>
#include <DirectXMath.h>

namespace RenderingEngine
{
	namespace ModelParser
	{
		typedef unsigned int shaderindextype;
		void ParseVertices(std::stringstream& myFile,
			const std::string& delimeter,
			unsigned int vertexCount,
			DirectX::XMVECTOR* vertices);

		void CalculatePerVertexNormals(unsigned int faceCount,
			const DirectX::XMVECTOR* const vertices,
			const shaderindextype* const indices, unsigned int vertexCount, DirectX::XMVECTOR* normals);

		void ParseVertexColors(std::stringstream& myFile, const std::string& delimeter, unsigned int vertexCount, DirectX::XMVECTOR* colors);
		void CalculatePerFaceStructures(const DirectX::XMVECTOR* const vertices,
			const DirectX::XMVECTOR* const normals,
			const DirectX::XMVECTOR* const colors,
			shaderindextype* indices,
			unsigned int faceCount,
			DirectX::XMVECTOR** newVertices,
			DirectX::XMVECTOR** newNormals,
			DirectX::XMVECTOR** newColors,
			unsigned int& vertexCount);
	}

}