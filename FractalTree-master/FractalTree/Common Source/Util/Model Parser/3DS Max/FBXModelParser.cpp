#include "pch.h"
#ifdef FBXSDK_SHARED
#include "FBXModelParser.h"
#include <fstream>
#include "FractalTreeRenderer.h"
#include "ASCIIModelParser.h"
#include "..\..\..\..\..\..\..\Program Files\Autodesk\FBX\FBX SDK\2014.0\include\fbxsdk\scene\animation\fbxanimstack.h"

FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
	switch(type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurb";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo:    return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

/**
 * Print the required number of tabs.
 */
int numTabs = 0;
void PrintTabs(std::ofstream& outFile) {
    for(int i = 0; i < numTabs; i++)
       // printf("\t");
		   outFile << "\t";
}
/**
 * Print an attribute.
 */
void PrintAttribute(FbxNodeAttribute* pAttribute, std::ofstream& outFile) {
    if(!pAttribute) return;
 
    FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
    FbxString attrName = pAttribute->GetName();
    PrintTabs(outFile);
    // Note: to retrieve the chararcter array of a FbxString, use its Buffer() method.
    //printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
	outFile << typeName.Buffer();
	outFile << attrName.Buffer();
}

/**
 * Print a node, its attributes, and all its children recursively.
 */
void PrintNode(FbxNode* pNode, std::ofstream& outFile) {
    PrintTabs(outFile);
    const char* nodeName = pNode->GetName();
    FbxDouble3  translation = pNode->LclTranslation.Get();
    FbxDouble3 rotation = pNode->LclRotation.Get();
    FbxDouble3 scaling = pNode->LclScaling.Get();

    // print the contents of the node.
    /*printf("<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n", 
        nodeName, 
        translation[0], translation[1], translation[2],
        rotation[0], rotation[1], rotation[2],
        scaling[0], scaling[1], scaling[2]
        );*/
	outFile << "<node name = " << nodeName << "translation = (" <<  translation[0] << ", " << translation[1] << ", " << translation[2] << ")";
//	outFile << "rotation = " << 
    numTabs++;

    // Print the node's attributes.
    for(int i = 0; i < pNode->GetNodeAttributeCount(); i++)
        PrintAttribute(pNode->GetNodeAttributeByIndex(i), outFile);

    // Recursively print the children nodes.
    for(int j = 0; j < pNode->GetChildCount(); j++)
        PrintNode(pNode->GetChild(j), outFile);

    numTabs--;
    PrintTabs(outFile);
	outFile << "</node>\n";
    //printf("</node>\n");
}

using namespace DirectX;

RenderingEngine::ModelBufferData* ParseFBXNode(FbxNode* node, RenderingEngine::FractalTreeRenderer* renderer, FbxScene* lScene)
{
	for (int i = 0; i < lScene->GetSrcObjectCount<FbxAnimStack>(); i++)
	{
		FbxAnimStack* lAnimStack = lScene->GetSrcObject<FbxAnimStack>(i);

		int nbAnimLayers = lAnimStack->GetMemberCount<FbxAnimLayer>();
		for (int l = 0; l < nbAnimLayers; l++)
		{
			FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(l);
			FbxAnimCurve* lAnimCurve = node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
			if(lAnimCurve)
			{
				int lKeyCount = lAnimCurve->KeyGetCount();
				for(int k = 0; k<lKeyCount; ++k)
				{
					FbxAnimCurveKey lKey = lAnimCurve->KeyGet(k);
					float lKeyValue = lKey.GetValue();
					switch(lKey.GetInterpolation())
					{
						case FbxNodeAttribute::eSubDiv:
							{

							}
							break;
						case FbxAnimCurveDef::eInterpolationLinear:
							{

							}
							break;
					}
				}
			}
		}
	}

	for(int i = 0; i < node->GetNodeAttributeCount(); ++i)
	{
		FbxNodeAttribute* attribute = node->GetNodeAttributeByIndex(i);
		switch(attribute->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			{
				FbxMesh* mesh = node->GetMesh();
				int count = mesh->GetPolygonCount();
				int indexCount = mesh->GetPolygonVertexCount();
				int cVertCount = mesh->GetControlPointsCount();
				int* indices = mesh->GetPolygonVertices();

				shaderindextype* indices2 = new shaderindextype[indexCount];
				for(int a = 0; a < indexCount; ++a)
				{
					indices2[a] = indices[a];
				}

				FbxVector4* vertices = mesh->GetControlPoints();

				XMVECTOR* verts = NULL;
				XMVECTOR* colors = NULL;
#ifndef _AMD64_
				verts = (XMVECTOR*)_aligned_malloc(sizeof(XMVECTOR) * cVertCount, 16);
				colors = (XMVECTOR*)_aligned_malloc(sizeof(XMVECTOR) * cVertCount, 16);
#else
				verts = new XMVECTOR[cVertCount];
				colors = new XMVECTOR[cVertCount];
#endif

				for(int a = 0; a < cVertCount; ++a)
				{
					verts[a] = XMVectorSet(static_cast<float>(vertices[a][1]), static_cast<float>(vertices[a][2]), static_cast<float>(vertices[a][0]), 0);
					colors[a] = XMVectorSet(1, 1, 1, 1);
				}

				bool useFaceNormals = true;
				if(useFaceNormals)
				{
					XMVECTOR* newVertices;
					XMVECTOR* newNormals;
					XMVECTOR* newColors;
					unsigned int vertexCount;

					RenderingEngine::ModelParser::CalculatePerFaceStructures(verts, NULL, colors, indices2, count, &newVertices, &newNormals, &newColors, vertexCount);
#ifndef _AMD64_
					_aligned_free(*verts);
					_aligned_free(*colors);
#else
					delete [] verts;
					delete [] colors;
#endif
					RenderingEngine::ModelBufferData* bufferData = renderer->CreateBufferDataFromVertexData(vertexCount, indexCount, newVertices, newColors, newNormals, indices2);
				//	delete[] indices;
					delete[] indices2;

#ifndef _AMD64_
					_aligned_free(newVertices);
					_aligned_free(newColors);
					_aligned_free(newNormals);
#else
					delete[] newVertices;

					delete[] newColors;
					delete[] newNormals;
#endif
					return bufferData;
				}

				return NULL;
			}
			break;
		}
	}

	return NULL;
}

RenderingEngine::ModelBufferData* ParseFBXModel(FbxManager* lSdkManager, FbxIOSettings *ios, const std::string& filePath, RenderingEngine::FractalTreeRenderer* renderer)
{
	// Create an importer using our sdk manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager,"");

	// Use the first argument as the filename for the importer.
	if(!lImporter->Initialize(filePath.c_str(), -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		exit(-1);
	}

	// Create a new scene so it can be populated by the imported file.
	FbxScene* lScene = FbxScene::Create(lSdkManager,"myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);
	int dir;
	lScene->GetGlobalSettings().GetAxisSystem().GetUpVector(dir); // this returns the equivalent of FbxAxisSystem::eYAxis

	// The file has been imported; we can get rid of the importer.
	lImporter->Destroy();
	//lScene->
	// Print the nodes of the scene and their attributes recursively.
	// Note that we are not printing the root node, because it should
	// not contain any attributes.
	FbxNode* lRootNode = lScene->GetRootNode();
	//std::ofstream outFile;
	//outFile.open("test");
// 	if(outFile.is_open())
// 	{
// 
// 	}

	
	for(int i = 0; i < lRootNode->GetChildCount(); i++)
	{
		auto nod = lRootNode->GetChild(i);
		//PrintNode(nod, outFile);
		return ParseFBXNode(nod, renderer, lScene);
	}

	//outFile.close();

	return NULL;
}
#endif