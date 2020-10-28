#pragma once
#ifdef FBXSDK_SHARED
#include <fbxsdk.h>

#include <string>

namespace RenderingEngine
{
	class FractalTreeRenderer;
	class ModelBufferData;
}

RenderingEngine::ModelBufferData* ParseFBXModel(FbxManager* lSdkManager, FbxIOSettings *ios, const std::string& filePath, RenderingEngine::FractalTreeRenderer* renderer);
#endif