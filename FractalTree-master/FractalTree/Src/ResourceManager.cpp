#include "pch.h"
#include "ResourceManager.h"
#include <fstream>
#include <sstream>
#include <stack>
#ifndef _NO_GRAPHICS
#include "..\Common Source\Rendering Engine\Rendering Engine Sample\SampleRenderer.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\ParticleSystem.h"
#endif

using namespace std;

map<string, ProgramVar*> ResourceManager::programVariables;
int ResourceManager::PlayerLocDebugIndex;
int ResourceManager::PlayerCellDebugIndex;
int ResourceManager::CellGenerationTimeIndex;
int ResourceManager::RenderTimeIndex;
int ResourceManager::SimulationTimeIndex;
int ResourceManager::CellCountIndex;
int ResourceManager::ObjectCountIndex;
int ResourceManager::cellGridWidthIndex;
bool ResourceManager::fastEverything = true;
bool ResourceManager::flyMove = false; //why
bool ResourceManager::UseLOD = true;
bool ResourceManager::RenderWireframe = false;
bool ResourceManager::alternateCellOrientation = true;
bool ResourceManager::unloadFarCells = false;
bool ResourceManager::burnTrees = false;
bool ResourceManager::useTerrainThread = false;
int ResourceManager::gridCellWidth = 5;
int ResourceManager::terrainTextureRepeatCount = 4; //it's destined to be 16 but whatever
float ResourceManager::MaxViewDistance = 200000.f;
bool ResourceManager::growthPaused = true;
int ResourceManager::cellGenerationCount = 0;
int ResourceManager::cellGenerationPerFrameCount = 1;
int ResourceManager::cellGenerationFrameInterval = 2;
float ResourceManager::lodDistanceFactor = 10000;
int ResourceManager::maxFoliageCount = 64000;
int ResourceManager::bladePerGrass = 2;
float ResourceManager::foliageRadiusSquared = 160000;
bool ResourceManager::enableFoliage = false;
int ResourceManager::maxTreeVertexCount = 1000;
int ResourceManager::maxTreeIndexCount = 1000;
bool ResourceManager::useShip = false;
float ResourceManager::cellSize = 2048;
bool ResourceManager::renderPrePass = false;
float ResourceManager::terrainLowest = -320000;
string ResourceManager::worldName = "TestWorld.gen";
float ResourceManager::startX = 0;
float ResourceManager::startZ = 0;
bool ResourceManager::useSky = true;
float ResourceManager::BallScaleFactor = 15;
bool ResourceManager::useSimplexNoise = true;
float ResourceManager::runSpeed = .25f;
bool ResourceManager::freeMove = false;
int ResourceManager::terrainThreadCount = 4;
bool ResourceManager::useWater = false;
bool ResourceManager::displayResourceVoxels = false;
bool ResourceManager::useLineMeshes = false;
bool ResourceManager::checkVoxels = true;
bool ResourceManager::enableGrowth = true;
unsigned int ResourceManager::treeUpdatePerFrame = 2;
float ResourceManager::nearZDistance = 1.01f;
float ResourceManager::farZDistance = 200000.f;
int ResourceManager::skeletonTestSegmentCount = 5;
bool ResourceManager::useGravity = true;
float ResourceManager::defaultDrag = .05f;
float ResourceManager::defaultRotationalDrag = .05f;
int ResourceManager::constraintIterationCount = 1000;
bool ResourceManager::showIKSkeleton = true;
float ResourceManager::forceUndoFactor = .9f;
bool ResourceManager::ignoreChainGravity = false;
float ResourceManager::gunForce = 50;
bool ResourceManager::solveIKInstantly = false;
bool ResourceManager::showCrosshair = false;
float ResourceManager::skeletonRot = 0;
float ResourceManager::skeletonRot2 = 0;
float ResourceManager::angleBB = .2f;
bool ResourceManager::useAngleMotor = true;
bool ResourceManager::useOtherAngle = false;
bool ResourceManager::isPaused = false;
bool ResourceManager::rotOnce = false;
unsigned int ResourceManager::maxFailCount = 50;
double ResourceManager::ikEpsilon = .001;
double ResourceManager::ikAlpha = .525;
int ResourceManager::ikIterationCount = 5;
bool ResourceManager::ikShowEnd = false;
bool ResourceManager::resetIK = false;
bool ResourceManager::onlyIkSolution = false;
bool ResourceManager::dynamicIk = false;
float ResourceManager::dotProductTolerance = .03f;
float ResourceManager::constraintMinError = .1f;
bool ResourceManager::constraintUseWarmStart = true;
int ResourceManager::timeStepCount = 1;
float ResourceManager::testForce = 1.f;
bool ResourceManager::makeChain = true;
#ifndef _NO_GRAPHICS
using namespace RenderingEngine;
vector<ModelBufferData*> ResourceManager::StaticModels;
map<string, int> ResourceManager::ResourceMap;
vector<string> ResourceManager::ModelList;
vector<bool> ResourceManager::ResourceLoaded;
int ResourceManager::modelCount = 0;
#endif

string ResourceManager::BaseResourceDirectory;

void ResourceManager::LoadResourceList(RenderingEngine::SampleRenderer* renderer, const string& fileName)
{
	ifstream resourceList(fileName);
	if(resourceList.is_open())
	{
		while(resourceList.good())
		{
			string resourceName;
			string resourceType;
			resourceList >> resourceName;
			resourceList >> resourceType;
#ifndef _NO_GRAPHICS
			if(resourceType == "model")
			{
				ResourceManager::loadModel(renderer, resourceName);
			}
#endif
		}
	}
	else
	{
		throw int();
	}

	resourceList.close();

#ifndef _NO_GRAPHICS
	ResourceManager::modelCount = static_cast<int>(ResourceManager::ModelList.size());
#ifdef FBXSDK_SHARED

	//FractalTreeResources::ModelList.push_back("testship2.FBX");
	//FractalTreeResources::ResourceMap["testship2.FBX"] = 4;
#endif
	ResourceManager::StaticModels.resize(ModelList.size() + 2);
	ResourceManager::ResourceLoaded.resize(ModelList.size() + 2, false);
	// Initialize the sdk manager. This object handles all our memory management.
#ifdef FBXSDK_SHARED
	FbxManager* lSdkManager = FbxManager::Create();

	// Create the io settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);
#endif


	ResourceManager::StaticModels[ModelList.size()] = renderer->CreateQuadModel();
	ResourceManager::StaticModels[ModelList.size() + 1] = renderer->CreateQuadModel2();
	ResourceManager::ResourceLoaded[ModelList.size()] = true;
	// Destroy the sdk manager and all other objects it was handling.
#ifdef FBXSDK_SHARED
	lSdkManager->Destroy();
#endif
#endif
}

#ifndef _NO_GRAPHICS
void ResourceManager::loadModel(RenderingEngine::SampleRenderer* renderer, const string& modelName)
{
	int whichResource = static_cast<int>(ResourceManager::ModelList.size());
	ResourceManager::ModelList.push_back(modelName);
	ResourceManager::ResourceMap[modelName] = whichResource;
	string modelFileName;
	string useVNormals;
	string invertNormals;

	ifstream modelDefinition(BaseResourceDirectory + "Resources/Models/" + modelName);

	if(modelDefinition.is_open())
	{
		modelDefinition >> modelFileName;
		modelDefinition >> modelFileName;

		modelDefinition >> useVNormals;
		modelDefinition >> useVNormals;

		modelDefinition >> invertNormals;
		modelDefinition >> invertNormals;

		string filePath = string(BaseResourceDirectory + "Resources/Models/" +  modelFileName);

		string exten = string(filePath, filePath.length() - 4, filePath.length());

		bool invertNorm = (invertNormals == "true");
		bool useVertexNorm = (useVNormals == "true");
		if(exten.compare(".ASE") == 0)
		{
			ifstream myFile(filePath, ios::ate);
			if(myFile.is_open())
			{
				size_t fileSize = myFile.tellg();
				myFile.seekg(0, ios::beg);
				char* shaderData = new char[fileSize];
				myFile.read(shaderData, fileSize);
				myFile.close();
				ResourceManager::StaticModels.push_back(renderer->CreateBufferDataFromString(shaderData, useVertexNorm, invertNorm));
				delete[] shaderData;
			}
			else
			{
				throw int();
			}

			modelDefinition.close();
		}
		else if(exten.compare(".FBX") == 0)
		{
#ifdef FBXSDK_SHARED
			ModelBufferData* buf = ParseFBXModel(lSdkManager, ios, filePath, renderer); 
			D3DRendererResources::StaticModels.push_back(buf);
#else
#endif
		}
	}
	else
	{
		throw int();
	}

}
#endif

void ResourceManager::InitializeProgramVariables()
{

	AddProgramVariable("terrainLowest", &ResourceManager::terrainLowest);
	AddProgramVariable("useTerrainThread", &ResourceManager::useTerrainThread);
	AddProgramVariable("unloadFarCells", &ResourceManager::unloadFarCells);
	AddProgramVariable("worldName", &ResourceManager::worldName);
	AddProgramVariable("startX", &ResourceManager::startX);
	AddProgramVariable("startZ", &ResourceManager::startZ);
	AddProgramVariable("useSky", &ResourceManager::useSky);
	AddProgramVariable("useSimplexNoise", &ResourceManager::useSimplexNoise);
	AddProgramVariable("runSpeed", &ResourceManager::runSpeed);
	AddProgramVariable("freeMove", &ResourceManager::freeMove);
	AddProgramVariable("terrainThreadCount", &ResourceManager::terrainThreadCount);
	AddProgramVariable("useWater", &ResourceManager::useWater);
	AddProgramVariable("skeletonTestSegmentCount", &ResourceManager::skeletonTestSegmentCount);
	AddProgramVariable("useGravity", &ResourceManager::useGravity);
	AddProgramVariable("defaultDrag", &ResourceManager::defaultDrag);
	AddProgramVariable("defaultRotationalDrag", &ResourceManager::defaultRotationalDrag);
	AddProgramVariable("constraintIterationCount", &ResourceManager::constraintIterationCount);
	AddProgramVariable("showIKSkeleton", &ResourceManager::showIKSkeleton);
	AddProgramVariable("forceUndoFactor", &ResourceManager::forceUndoFactor);
	AddProgramVariable("ignoreChainGravity", &ResourceManager::ignoreChainGravity);
	AddProgramVariable("gunForce", &ResourceManager::gunForce);
	AddProgramVariable("solveIKInstantly", &ResourceManager::solveIKInstantly);
	AddProgramVariable("showCrosshair", &ResourceManager::showCrosshair);
	AddProgramVariable("angleBB", &ResourceManager::angleBB);
	AddProgramVariable("maxFailCount", &ResourceManager::maxFailCount);
	AddProgramVariable("ikEpsilon", &ResourceManager::ikEpsilon);
	AddProgramVariable("ikAlpha", &ResourceManager::ikAlpha);
	AddProgramVariable("ikIterationCount", &ResourceManager::ikIterationCount);
	AddProgramVariable("ikShowEnd", &ResourceManager::ikShowEnd);
	AddProgramVariable("onlyIkSolution", &ResourceManager::onlyIkSolution);
	AddProgramVariable("dynamicIk", &ResourceManager::dynamicIk);
	AddProgramVariable("ikDotProductTolerance", &ResourceManager::dotProductTolerance);
	AddProgramVariable("constraintMinError", &ResourceManager::constraintMinError);
	AddProgramVariable("constraintUseWarmStart", &ResourceManager::constraintUseWarmStart);
	AddProgramVariable("timeStepCount", &ResourceManager::timeStepCount);
	AddProgramVariable("testForce", &ResourceManager::testForce);
	AddProgramVariable("makeChain", &ResourceManager::makeChain);
}

void ResourceManager::LoadProgramVariables(const string& fileName, map<string, ProgramVar*>* varTarget)
{
	//Register program variables


	//Load possible values from config file
	ifstream inputFile(fileName);

	while(inputFile.good())
	{
		string varName;
		inputFile >> varName;

		string varType;
		inputFile >> varType;

		if(varType == "int")
		{
			int val;
			inputFile >> val;
			if (varTarget == NULL)
				ResourceManager::programVariables[varName]->SetValue(&val);
			else
				(*varTarget)[varName]->SetValue(&val);
		}
		else if(varType == "float")
		{
			float val;
			inputFile >> val;
			if (varTarget == NULL)
				ResourceManager::programVariables[varName]->SetValue(&val);
			else
				(*varTarget)[varName]->SetValue(&val);
		}
		else if (varType == "double")
		{
			double val;
			inputFile >> val;
			if (varTarget == NULL)
				ResourceManager::programVariables[varName]->SetValue(&val);
			else
				(*varTarget)[varName]->SetValue(&val);
		}
		else if(varType == "bool")
		{
			bool val;
			string valString;
			inputFile >> valString;
			val = (valString == "true");
			if (varTarget == NULL)
				ResourceManager::programVariables[varName]->SetValue(&val);
			else
				(*varTarget)[varName]->SetValue(&val);
		}
		else if (varType == "string")
		{
			string valString;
			inputFile >> valString;
			if (varTarget == NULL)
				ResourceManager::programVariables[varName]->SetValue(&valString);
			else
				(*varTarget)[varName]->SetValue(&valString);
		}
		else
		{
			throw int();
		}
	}
}

#ifndef _NO_GRAPHICS
using namespace RenderingEngine;

FontDefinition* ResourceManager::LoadFontDefinition(stringstream& inputFile)
{
	FontDefinition* fontDefinition = NULL;

	if(inputFile.good())
	{
		fontDefinition = new FontDefinition();
		int index = 0;
		while(inputFile.good())
		{
			float texCoord1;
			float texCoord2;
			int pWidth;
			int wastedSpace;
			char wastedSpace2;
			inputFile >> wastedSpace;
			if(index > 0)
				inputFile >> wastedSpace2;
			inputFile >> texCoord1;
			inputFile >> texCoord2;
			inputFile >> pWidth;

			fontDefinition->characterDefinitions[wastedSpace].character = wastedSpace;
			fontDefinition->characterDefinitions[wastedSpace].textureCoordinates[0] = texCoord1;
			fontDefinition->characterDefinitions[wastedSpace].textureCoordinates[1] = texCoord2;
			fontDefinition->characterDefinitions[wastedSpace].pixelWidth = pWidth;
			++index;
		}
	}
	else
	{
		return NULL;
	}

	return fontDefinition;
}

void ResourceManager::LoadParticleEffect(const string& fileName, RenderingEngine::ParticleSystemDefinition* particleSystemDefinition)
{
	map<string, void*> fieldPointers;

	fieldPointers["maxConcurrentParticles"] = 
		&particleSystemDefinition->maxConcurrentParticles;
	fieldPointers["emitCountMin"] = 
		&particleSystemDefinition->emitCountMin;
	fieldPointers["emitCountRange"] = 
		&particleSystemDefinition->emitCountRange;
	fieldPointers["emitCooldown"] = 
		&particleSystemDefinition->emitCooldown;
	fieldPointers["particleDuration"] =
		&particleSystemDefinition->particleDuration;
	fieldPointers["emitPerDirectionCount"] = 
		&particleSystemDefinition->emitPerDirectionCount;
	fieldPointers["delay"] =
		&particleSystemDefinition->delay;
	fieldPointers["particleInitialSpeedMin"] =
		&particleSystemDefinition->particleInitialSpeedMin;
	fieldPointers["particleInitialSpeedMax"] =
		&particleSystemDefinition->particleInitialSpeedMax;
	fieldPointers["particleRadiusFactor"] = 
		&particleSystemDefinition->particleRadiusFactor;
	fieldPointers["particleRadius"] = 
		&particleSystemDefinition->particleRadius;
	fieldPointers["particleRadiusMin"] =
		&particleSystemDefinition->particleRadiusMin;
	fieldPointers["particleAccelerationDirection"] =
		&particleSystemDefinition->particleAccelerationDirection;
	fieldPointers["particleInitialAccelerationMagnitude"] = 
		&particleSystemDefinition->particleInitialAccelerationMagnitude;
	fieldPointers["particleCreationOffsetAngle"] =
		&particleSystemDefinition->particleCreationOffsetAngle;
	fieldPointers["particleCreationOffsetDistance"] =
		&particleSystemDefinition->particleCreationOffsetDistance;
	fieldPointers["particleEmissionVelocityRotation"] =
		&particleSystemDefinition->particleEmissionVelocityRotation;
	fieldPointers["particleInitialVelocityRotationRange"] =
		&particleSystemDefinition->particleInitialVelocityRotationRange;
	fieldPointers["particleEmissionDirection"] = 
		&particleSystemDefinition->particleEmissionDirection;
	fieldPointers["rotateSystem"] =
		&particleSystemDefinition->rotateSystem;
	fieldPointers["killSoon"] =
		&particleSystemDefinition->killSoon;
	fieldPointers["life"] =
		&particleSystemDefinition->life;

	ifstream inputFile(fileName);

	stack<string> tokenStack;

	int whichPart = 0;

	string varName;
	string varType;
	string varVal;

	while(inputFile.good())
	{	
		if(whichPart == 0)
		{
			string currentToken;
			inputFile >> currentToken;

			if(currentToken.compare("{") == 0)
			{
				tokenStack.push("{");
			}
			else if (currentToken.compare("}") == 0)
			{
				if(tokenStack.top().compare("{") == 0)
				{
					tokenStack.pop();
				}
				else
				{
					throw int();
				}
			}
			else
			{
				varName = currentToken;
				++whichPart;
			}		
		}
		else if(whichPart == 1)
		{
			inputFile >> varType;
			++whichPart;
		}
		else if(whichPart == 2)
		{
			whichPart = 0;
			using namespace DirectX;

			if(varType.compare("float") == 0)
			{
				float val;
				inputFile >> val;
				*static_cast<float*>(fieldPointers[varName]) = val;
			}
			else if(varType.compare("uint") == 0)
			{
				unsigned int val;
				inputFile >> val;
				*static_cast<unsigned int*>(fieldPointers[varName]) = val;
			}
			else if(varType.compare("int") == 0)
			{
				int val;
				inputFile >> val;
				*static_cast<int*>(fieldPointers[varName]) = val;
			}
			else if(varType.compare("XMVECTOR") == 0)
			{
				float x, y, z, w;
				inputFile >> x;
				inputFile >> y;
				inputFile >> z;
				inputFile >> w;

				*static_cast<XMVECTOR*>(fieldPointers[varName]) = XMVectorSet(x, y, z, w);
			}
			else if(varType.compare("list_XMVECTOR") == 0)
			{
				float x, y, z, w;
				inputFile >> x;
				inputFile >> y;
				inputFile >> z;
				inputFile >> w;
				(*static_cast<vector<XMVECTOR>* >(fieldPointers[varName])).push_back(XMVectorSet(x, y, z, w));
			}
			else if(varType.compare("bool") == 0)
			{
				bool val;
				inputFile >> val;
				*static_cast<bool*>(fieldPointers[varName]) = val;
			}
			else if(varType.compare("XMFLOAT3") == 0)
			{
				float x, y, z;
				inputFile >> x;
				inputFile >> y;
				inputFile >> z;

				*static_cast<XMFLOAT3*>(fieldPointers[varName]) = XMFLOAT3(x, y, z);
			}
		}			

	}
}
#endif
void ResourceManager::AddProgramVariable(const string& varName, int* linkedVariable, map<string, ProgramVar*>* varTarget)
{
	ProgramVarInt* newVar = new ProgramVarInt();
	newVar->varPointer = linkedVariable;
	if (varTarget == NULL)
		ResourceManager::programVariables[varName] = newVar;
	else
		(*varTarget)[varName] = newVar;
}

void ResourceManager::AddProgramVariable(const string& varName, unsigned int* linkedVariable, map<string, ProgramVar*>* varTarget)
{
	ProgramVarInt* newVar = new ProgramVarInt();
	newVar->varPointer = linkedVariable;
	if (varTarget == NULL)
		ResourceManager::programVariables[varName] = newVar;
	else
		(*varTarget)[varName] = newVar;
}

void ResourceManager::AddProgramVariable(const string& varName, float* linkedVariable, map<string, ProgramVar*>* varTarget)
{
	ProgramVarFloat* newVar = new ProgramVarFloat();
	newVar->varPointer = linkedVariable;
	if (varTarget == NULL)
		ResourceManager::programVariables[varName] = newVar;
	else
		(*varTarget)[varName] = newVar;
}
void ResourceManager::AddProgramVariable(const string& varName, double* linkedVariable, map<string, ProgramVar*>* varTarget)
{
	ProgramVarDouble* newVar = new ProgramVarDouble();
	newVar->varPointer = linkedVariable;
	if (varTarget == NULL)
		ResourceManager::programVariables[varName] = newVar;
	else
		(*varTarget)[varName] = newVar;
}
void ResourceManager::AddProgramVariable(const string& varName, bool* linkedVariable, map<string, ProgramVar*>* varTarget)
{
	ProgramVarBool* newVar = new ProgramVarBool();
	newVar->varPointer = linkedVariable;
	if (varTarget == NULL)
		ResourceManager::programVariables[varName] = newVar;
	else
		(*varTarget)[varName] = newVar;
}
void ResourceManager::AddProgramVariable(const string& varName, string* linkedVariable, map<string, ProgramVar*>* varTarget)
{
	ProgramVarString* newVar = new ProgramVarString();
	newVar->varPointer = linkedVariable;
	if (varTarget == NULL)
		ResourceManager::programVariables[varName] = newVar;
	else
		(*varTarget)[varName] = newVar;
}