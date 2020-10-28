#include "pch.h"
#include "ASCIIModelParser.h"

using namespace DirectX;
using namespace std;

namespace RenderingEngine { namespace ModelParser
{
	void ParseVertices(stringstream& myFile, const string& delimeter, unsigned int vertexCount, XMVECTOR* vertices)
	{
		bool foundAllVertices = false;
		int whichVert = 0;
		int whichLine = 0;
		while(!foundAllVertices && myFile.good())
		{
			string line;
			getline(myFile, line);
			int pos = static_cast<int>(line.find(delimeter));
			if(pos != string::npos)
			{
				XMVECTOR newVertex;
				string newStr = line.substr(pos + 14);
				newStr = newStr.substr(newStr.find_first_not_of(" "));
				string l = newStr.substr(0, newStr.find_first_not_of("0123456789"));
				{
					istringstream intStream = istringstream(l);

					intStream >> whichLine;
				}

				float xCoord, yCoord, zCoord;

				newStr = newStr.substr((newStr.find_first_not_of("0123456789")));

				newStr = newStr.substr((newStr.find_first_not_of(9)));
				string xC = newStr.substr(0, newStr.find_first_not_of("-0123456789."));
				{
					istringstream intStream(xC);
					intStream >> xCoord;
				}

				newStr = newStr.substr((newStr.find_first_of(9)));
				newStr = newStr.substr((newStr.find_first_not_of(9)));
				string yC = newStr.substr(0, newStr.find_first_of(9));
				{
					istringstream intStream(yC);
					intStream >> yCoord;
				}

				newStr = newStr.substr((newStr.find_first_of(9)));
				newStr = newStr.substr((newStr.find_first_not_of(9)));
				string zC = newStr.substr(0, newStr.find_first_not_of("-.0123456789"));
				{
					istringstream intStream(zC);
					intStream >> zCoord;
				}

				newVertex = XMVectorSet(yCoord, zCoord, xCoord, 0);

				if(whichVert == vertexCount - 1)
				{
					foundAllVertices = true;
				}

				vertices[whichVert] = newVertex;
				++whichVert;
			}	
		}
		if(!myFile.good())
		{
			int bb = 0;
			++bb;
		}
	}

	void CalculatePerVertexNormals(unsigned int faceCount, const XMVECTOR* const vertices, const shaderindextype* const indices, unsigned int vertexCount, XMVECTOR* normals)
	{
		for(unsigned int a = 0; a < vertexCount; ++a)
		{
			normals[a] = XMVectorSet(0, 0, 0, 0);
		}

		for(unsigned int ind = 0; ind < faceCount; ++ind)
		{
			int xasdf = indices[ind*3];
			XMVECTOR v1 = vertices[indices[ind*3]];
			XMVECTOR v2 = vertices[indices[ind*3+1]];
			XMVECTOR v33 = vertices[indices[ind*3+2]];
			XMVECTOR normal = XMVector3Cross(XMVectorSubtract(v2, v1), XMVectorSubtract(v33, v1));

			normals[indices[ind*3]] = XMVectorAdd(normals[indices[ind*3]], normal);

			normals[indices[ind*3 + 1]] = XMVectorAdd(normals[indices[ind*3 + 1]], normal);

			normals[indices[ind*3 + 2]] = XMVectorAdd(normals[indices[ind*3 + 2]], normal);

		}

		for(unsigned int ind = 0; ind < vertexCount; ++ind)
		{
			normals[ind] = XMVector3Normalize(normals[ind]);
		}
	}

	void ParseVertexColors(std::stringstream& myFile, const std::string& delimeter, unsigned int vertexCount, XMVECTOR* colors)
	{
		bool foundAllColors = false;
		int whichLine = 0;
		while(!foundAllColors)
		{	
			XMVECTOR newColor = XMVectorSet(1,1,1,1);

			if(whichLine == vertexCount - 1)
			{
				foundAllColors = true;
			}
			colors[whichLine] = newColor;
			++whichLine;
		}
	}

	void CalculatePerFaceStructures(const XMVECTOR* const vertices, const XMVECTOR* const normals, const XMVECTOR* const colors, shaderindextype* indices, unsigned int faceCount, XMVECTOR** newVertices, XMVECTOR** newNormals, XMVECTOR** newColors, unsigned int& vertexCount)
	{
		vertexCount = faceCount*3;
		*newVertices = new XMVECTOR[vertexCount];
		*newNormals = new XMVECTOR[vertexCount];
		*newColors = new XMVECTOR[vertexCount];

		for(unsigned int whichVertex = 0; whichVertex < faceCount; ++whichVertex)
		{
			(*(newVertices))[whichVertex*3] = vertices[indices[whichVertex*3]];
			(*(newVertices))[whichVertex*3 + 1] = vertices[indices[whichVertex*3 + 1]];
			(*(newVertices))[whichVertex*3 + 2] = vertices[indices[whichVertex*3 + 2]];

			(*(newColors))[whichVertex*3] = colors[indices[whichVertex*3]];
			(*(newColors))[whichVertex*3 + 1] = colors[indices[whichVertex*3 + 1]];
			(*(newColors))[whichVertex*3 + 2] = colors[indices[whichVertex*3 + 2]];

			XMVECTOR newNormal = XMVector3Normalize(XMVector3Cross(XMVectorSubtract((*newVertices)[whichVertex*3 + 1], (*newVertices)[whichVertex*3]),
				XMVectorSubtract((*newVertices)[whichVertex*3 + 2], (*newVertices)[whichVertex*3])));
			(*(newNormals))[whichVertex*3] = newNormal;
			(*(newNormals))[whichVertex*3 + 1] = newNormal;
			(*(newNormals))[whichVertex*3 + 2] = newNormal;
		}

		for(unsigned int whichIndex = 0; whichIndex < vertexCount; ++whichIndex)
		{
			indices[whichIndex] = whichIndex;
		}
	}
}}