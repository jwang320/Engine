#pragma once
#include <map>
#include <vector>

namespace RenderingEngine
{
	struct ParticleSystemDefinition;
	struct FontDefinition;
	class ModelBufferData;
	class SampleRenderer;
}

struct ProgramVar
{
	void* varPointer;
	virtual void SetValue(void* val) = 0;
};

struct ProgramVarInt : public ProgramVar
{
	virtual void SetValue(void* val) override
	{
		int castVal = *(static_cast<int*>(val));

		int* thisVal = static_cast<int*>(varPointer);
		*thisVal = castVal;
	}
};

struct ProgramVarUInt : public ProgramVar
{
	virtual void SetValue(void* val) override
	{
		unsigned int castVal = *(static_cast<unsigned int*>(val));

		unsigned int* thisVal = static_cast<unsigned int*>(varPointer);
		*thisVal = castVal;
	}
};

struct ProgramVarFloat : public ProgramVar
{
	virtual void SetValue(void* val) override
	{
		float castVal = *(static_cast<float*>(val));

		float* thisVal = static_cast<float*>(varPointer);
		*thisVal = castVal;
	}
};
struct ProgramVarBool : public ProgramVar
{
	virtual void SetValue(void* val) override
	{
		bool castVal = *(static_cast<bool*>(val));

		bool* thisVal = static_cast<bool*>(varPointer);
		*thisVal = castVal;
	}
};
struct ProgramVarString : public ProgramVar
{
	virtual void SetValue(void* val) override
	{
		std::string castVal = *(static_cast<std::string*>(val));

		std::string* thisVal = static_cast<std::string*>(varPointer);
		*thisVal = castVal;
	}
};
struct ProgramVarDouble : public ProgramVar
{
	virtual void SetValue(void* val) override
	{
		double castVal = *(static_cast<double*>(val));

		double* thisVal = static_cast<double*>(varPointer);
		*thisVal = castVal;
	}
};

class ResourceManager
{
public:
	static std::map<std::string, ProgramVar*> programVariables;

#ifndef _NO_GRAPHICS
	static std::vector<bool> ResourceLoaded;
	static std::vector<std::string> ModelList;
	static std::map<std::string, int> ResourceMap;
	static std::vector<RenderingEngine::ModelBufferData*> StaticModels;
#endif

public:
	static void InitializeProgramVariables();
	static void LoadProgramVariables(const std::string& fileName, std::map<std::string, ProgramVar*>* varTarget = NULL);
	static void LoadResourceList(RenderingEngine::SampleRenderer* renderer, const std::string& fileName);
#ifndef _NO_GRAPHICS
	static void loadModel(RenderingEngine::SampleRenderer* renderer, const std::string& modelName);
	static void LoadParticleEffect(const std::string& fileName, RenderingEngine::ParticleSystemDefinition* particleSystemDefinition);
	static RenderingEngine::FontDefinition* LoadFontDefinition(std::stringstream& inputFile);
#endif
	static void AddProgramVariable(const std::string& varName, int* linkedVariable, std::map<std::string, ProgramVar*>* varTarget = nullptr);
	static void AddProgramVariable(const std::string& varName, unsigned int* linkedVariable, std::map<std::string, ProgramVar*>* varTarget = NULL);
	static void AddProgramVariable(const std::string& varName, float* linkedVariable, std::map<std::string, ProgramVar*>* varTarget = NULL);
	static void AddProgramVariable(const std::string& varName, double* linkedVariable, std::map<std::string, ProgramVar*>* varTarget = NULL);
	static void AddProgramVariable(const std::string& varName, bool* linkedVariable, std::map<std::string, ProgramVar*>* varTarget = NULL);
	static void AddProgramVariable(const std::string& varName, std::string* linkedVariable, std::map<std::string, ProgramVar*>* varTarget = NULL);

public:
	static int modelCount;
	static std::string BaseResourceDirectory;


	static bool useGravity;
	static int skeletonTestSegmentCount;
	static int PlayerLocDebugIndex;
	static int PlayerCellDebugIndex;
	static int CellGenerationTimeIndex;
	static int RenderTimeIndex;
	static int SimulationTimeIndex;
	static int CellCountIndex;
	static bool fastEverything;
	static bool resetIK;
	static bool onlyIkSolution;
	static bool flyMove;
	static bool UseLOD;
	static bool RenderWireframe;
	static bool alternateCellOrientation;
	static bool useTerrainThread;
	static float MaxViewDistance;
	static int ObjectCountIndex;
	static int gridCellWidth;
	static int cellGridWidthIndex;
	static bool burnTrees;
	static int terrainTextureRepeatCount;
	static bool growthPaused;
	static int cellGenerationCount;
	static float lodDistanceFactor;
	static int cellGenerationPerFrameCount;
	static int cellGenerationFrameInterval;
	static int maxFoliageCount;
	static int bladePerGrass;
	static float foliageRadiusSquared;
	static bool enableFoliage;
	static int maxTreeVertexCount;
	static int maxTreeIndexCount;
	static bool useShip;
	static float cellSize;
	static bool renderPrePass;
	static float terrainLowest;
	static bool unloadFarCells;
	static std::string worldName;
	static float startX;
	static float startZ;
	static bool useSky;
	static float BallScaleFactor;
	static bool useSimplexNoise;
	static float runSpeed;
	static bool freeMove;
	static int terrainThreadCount;
	static bool useWater;
	static bool displayResourceVoxels;
	static bool useLineMeshes;
	static bool checkVoxels;
	static bool enableGrowth;
	static unsigned int treeUpdatePerFrame;
	static int constraintIterationCount;
	static int timeStepCount;
	static float nearZDistance;
	static float farZDistance;
	static float defaultDrag;
	static float defaultRotationalDrag;
	static bool showIKSkeleton;
	static float forceUndoFactor;
	static bool ignoreChainGravity;
	static float gunForce;
	static bool solveIKInstantly;
	static bool showCrosshair;
	static float skeletonRot;
	static float skeletonRot2;
	static float angleBB;
	static bool useAngleMotor;
	static bool useOtherAngle;
	static bool isPaused;
	static bool rotOnce;
	static unsigned int maxFailCount;
	static double ikEpsilon;
	static double ikAlpha;
	static int ikIterationCount;
	static bool ikShowEnd;
	static bool dynamicIk;
	static float dotProductTolerance;
	static float constraintMinError;
	static bool constraintUseWarmStart;
	static float testForce;
	static bool makeChain;
};