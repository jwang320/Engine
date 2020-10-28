#pragma once
#include "ResourceManager.h"
#ifndef _NO_GRAPHICS

namespace RenderingEngine
{
	class FractalTreeRenderer;
	class ModelBufferData;
	class StandardRenderer;
}

namespace SimpleUI
{
	class UITextOutputBox;
}
#endif


namespace ProceduralGeneration
{
	class GeneratorDefinition;
}


class FractalTreeResources : public ResourceManager
{
public:
	enum ParticleSystems
	{
		BasicFireTrail,
		IneffectiveMissileFireTrail,
		DecentMissileFireTrail,
		MediumExplosion,
		HugeExplosion,
		GrenadeFireTrail,
		GiantExplosion2,
		HitExplosion
	};
#ifndef _NO_GRAPHICS
	static RenderingEngine::ParticleSystemDefinition ParticleSystemDefinitions[10];
	static void LoadResources(RenderingEngine::FractalTreeRenderer* renderer);
	static void LoadOtherResources();
	static void InitializeProgramVariables();
#define WCOUT *FractalTreeResources::ConsoleOutput
	static SimpleUI::UITextOutputBox* ConsoleOutput;

private:
	

#endif
public:
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
	static void ConsolePrintLine(const std::string& line);
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
	static float nearZDistance;
	static float farZDistance;
	static float defaultDrag;
	static float defaultRotationalDrag;
	static int constraintIterationCount;
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
	static int timeStepCount;
	static float testForce;
	static bool makeChain;
};


