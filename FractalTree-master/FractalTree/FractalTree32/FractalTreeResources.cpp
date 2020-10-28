#include "pch.h"
#include "FractalTreeResources.h"
#include <fstream>
#ifndef _NO_GRAPHICS
#include "FractalTreeRenderer.h"
#include <stack>
#include <vector>
using namespace RenderingEngine;
#endif
#ifdef FBXSDK_SHARED
#include <fbxsdk.h>
#include "Rendering Engine\FBXModelParser.h"
#ifndef _DEBUG
#pragma comment(lib, fbxsdk-2014.0.lib")
#endif
#endif
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\ParticleSystem.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\StandardRendererResources.h"

using namespace std;
using namespace ProceduralGeneration;

int FractalTreeResources::PlayerLocDebugIndex;
int FractalTreeResources::PlayerCellDebugIndex;
int FractalTreeResources::CellGenerationTimeIndex;
int FractalTreeResources::RenderTimeIndex;
int FractalTreeResources::SimulationTimeIndex;
int FractalTreeResources::CellCountIndex;
int FractalTreeResources::ObjectCountIndex;
int FractalTreeResources::cellGridWidthIndex;
bool FractalTreeResources::fastEverything = true;
bool FractalTreeResources::flyMove = false; //why
bool FractalTreeResources::UseLOD = true;
bool FractalTreeResources::RenderWireframe = false;
bool FractalTreeResources::alternateCellOrientation = true;
bool FractalTreeResources::unloadFarCells = false;
bool FractalTreeResources::burnTrees = false;
bool FractalTreeResources::useTerrainThread = false;
int FractalTreeResources::gridCellWidth = 5;
int FractalTreeResources::terrainTextureRepeatCount = 4; //it's destined to be 16 but whatever
float FractalTreeResources::MaxViewDistance = 200000.f;
bool FractalTreeResources::growthPaused = true;
int FractalTreeResources::cellGenerationCount = 0;
int FractalTreeResources::cellGenerationPerFrameCount  = 1;
int FractalTreeResources::cellGenerationFrameInterval = 2;
float FractalTreeResources::lodDistanceFactor = 10000;
int FractalTreeResources::maxFoliageCount = 64000;
int FractalTreeResources::bladePerGrass = 2;
float FractalTreeResources::foliageRadiusSquared = 160000;
bool FractalTreeResources::enableFoliage = false;
int FractalTreeResources::maxTreeVertexCount = 1000;
int FractalTreeResources::maxTreeIndexCount = 1000;
bool FractalTreeResources::useShip = false;
float FractalTreeResources::cellSize = 2048;
bool FractalTreeResources::renderPrePass = false;
float FractalTreeResources::terrainLowest = -320000;
string FractalTreeResources::worldName = "TestWorld.gen";
float FractalTreeResources::startX = 0;
float FractalTreeResources::startZ = 0;
bool FractalTreeResources::useSky = true;
float FractalTreeResources::BallScaleFactor = 15;
bool FractalTreeResources::useSimplexNoise = true;
float FractalTreeResources::runSpeed = .25f;
bool FractalTreeResources::freeMove = false;
int FractalTreeResources::terrainThreadCount = 4;
bool FractalTreeResources::useWater = false;
bool FractalTreeResources::displayResourceVoxels = false;
bool FractalTreeResources::useLineMeshes = false;
bool FractalTreeResources::checkVoxels = true;
bool FractalTreeResources::enableGrowth = true;
unsigned int FractalTreeResources::treeUpdatePerFrame = 2;
float FractalTreeResources::nearZDistance = 1.01f;
float FractalTreeResources::farZDistance = 200000.f;
int FractalTreeResources::skeletonTestSegmentCount = 5;
bool FractalTreeResources::useGravity = true;
float FractalTreeResources::defaultDrag = .05f;
float FractalTreeResources::defaultRotationalDrag = .05f;
int FractalTreeResources::constraintIterationCount = 1000;
bool FractalTreeResources::showIKSkeleton = true;
float FractalTreeResources::forceUndoFactor = .9f;
bool FractalTreeResources::ignoreChainGravity = false;
float FractalTreeResources::gunForce = 50;
bool FractalTreeResources::solveIKInstantly = false;
bool FractalTreeResources::showCrosshair = false;
float FractalTreeResources::skeletonRot = 0;
float FractalTreeResources::skeletonRot2 = 0;
float FractalTreeResources::angleBB = .2f;
bool FractalTreeResources::useAngleMotor = true;
bool FractalTreeResources::useOtherAngle = false;
bool FractalTreeResources::isPaused = false;
bool FractalTreeResources::rotOnce = false;
unsigned int FractalTreeResources::maxFailCount = 50;
double FractalTreeResources::ikEpsilon = .001;
double FractalTreeResources::ikAlpha = .525;
int FractalTreeResources::ikIterationCount = 5;
bool FractalTreeResources::ikShowEnd = false;
bool FractalTreeResources::resetIK = false;
bool FractalTreeResources::onlyIkSolution = false;
bool FractalTreeResources::dynamicIk = false;
float FractalTreeResources::dotProductTolerance = .03f;
float FractalTreeResources::constraintMinError = .1f;
bool FractalTreeResources::constraintUseWarmStart = true;
int FractalTreeResources::timeStepCount = 1;
float FractalTreeResources::testForce = 1.f;
bool FractalTreeResources::makeChain = true;

#ifndef _NO_GRAPHICS
#include "..\Common Source\SimpleUI\UITextOutputBox.h"
ParticleSystemDefinition FractalTreeResources::ParticleSystemDefinitions[10];
SimpleUI::UITextOutputBox* FractalTreeResources::ConsoleOutput = NULL;
#endif
#ifndef _NO_GRAPHICS

void FractalTreeResources::ConsolePrintLine(const string& line)
{
	if(FractalTreeResources::ConsoleOutput != NULL)
	{
		FractalTreeResources::ConsoleOutput->PrintLine(line);
	}
}

void FractalTreeResources::InitializeProgramVariables()
{
	AddProgramVariable("cellGenerationCount", &FractalTreeResources::cellGenerationCount);
	AddProgramVariable("cellGenerationPerFrameCount", &FractalTreeResources::cellGenerationPerFrameCount);
	AddProgramVariable("cellGenerationFrameInterval", &FractalTreeResources::cellGenerationFrameInterval);
	AddProgramVariable("lodDistanceFactor", &FractalTreeResources::lodDistanceFactor);
	AddProgramVariable("maxFoliageCount", &FractalTreeResources::maxFoliageCount);
	AddProgramVariable("bladePerGrass", &FractalTreeResources::bladePerGrass);
	AddProgramVariable("foliageRadiusSquared", &FractalTreeResources::foliageRadiusSquared);
	AddProgramVariable("enableFoliage", &FractalTreeResources::enableFoliage);
	AddProgramVariable("maxTreeVertexCount", &FractalTreeResources::maxTreeVertexCount);
	AddProgramVariable("maxTreeIndexCount", &FractalTreeResources::maxTreeIndexCount);
	AddProgramVariable("useShip", &FractalTreeResources::useShip);
	AddProgramVariable("cellSize", &FractalTreeResources::cellSize);
	AddProgramVariable("renderPrePass", &StandardRendererResources::renderPrePass);
	AddProgramVariable("terrainLowest", &FractalTreeResources::terrainLowest);
	AddProgramVariable("useTerrainThread", &FractalTreeResources::useTerrainThread);
	AddProgramVariable("unloadFarCells", &FractalTreeResources::unloadFarCells);
	AddProgramVariable("worldName", &FractalTreeResources::worldName);
	AddProgramVariable("startX", &FractalTreeResources::startX);
	AddProgramVariable("startZ", &FractalTreeResources::startZ);
	AddProgramVariable("useSky", &FractalTreeResources::useSky);
	AddProgramVariable("useSimplexNoise", &FractalTreeResources::useSimplexNoise);
	AddProgramVariable("runSpeed", &FractalTreeResources::runSpeed);
	AddProgramVariable("freeMove", &FractalTreeResources::freeMove);
	AddProgramVariable("terrainThreadCount", &FractalTreeResources::terrainThreadCount);
	AddProgramVariable("useWater", &FractalTreeResources::useWater);
	AddProgramVariable("skeletonTestSegmentCount", &FractalTreeResources::skeletonTestSegmentCount);
	AddProgramVariable("useGravity", &FractalTreeResources::useGravity);
	AddProgramVariable("defaultDrag", &FractalTreeResources::defaultDrag);
	AddProgramVariable("defaultRotationalDrag", &FractalTreeResources::defaultRotationalDrag);
	AddProgramVariable("constraintIterationCount", &FractalTreeResources::constraintIterationCount);
	AddProgramVariable("showIKSkeleton", &FractalTreeResources::showIKSkeleton);
	AddProgramVariable("forceUndoFactor", &FractalTreeResources::forceUndoFactor);
	AddProgramVariable("ignoreChainGravity", &FractalTreeResources::ignoreChainGravity);
	AddProgramVariable("gunForce", &FractalTreeResources::gunForce);
	AddProgramVariable("solveIKInstantly", &FractalTreeResources::solveIKInstantly);
	AddProgramVariable("showCrosshair", &FractalTreeResources::showCrosshair);
	AddProgramVariable("angleBB", &FractalTreeResources::angleBB);
	AddProgramVariable("maxFailCount", &FractalTreeResources::maxFailCount);
	AddProgramVariable("ikEpsilon", &FractalTreeResources::ikEpsilon);
	AddProgramVariable("ikAlpha", &FractalTreeResources::ikAlpha);
	AddProgramVariable("ikIterationCount", &FractalTreeResources::ikIterationCount);
	AddProgramVariable("ikShowEnd", &FractalTreeResources::ikShowEnd);
	AddProgramVariable("onlyIkSolution", &FractalTreeResources::onlyIkSolution);
	AddProgramVariable("dynamicIk", &FractalTreeResources::dynamicIk);
	AddProgramVariable("ikDotProductTolerance", &FractalTreeResources::dotProductTolerance);
	AddProgramVariable("constraintMinError", &FractalTreeResources::constraintMinError);
	AddProgramVariable("constraintUseWarmStart", &FractalTreeResources::constraintUseWarmStart);
	AddProgramVariable("timeStepCount", &FractalTreeResources::timeStepCount);
	AddProgramVariable("testForce", &FractalTreeResources::testForce);
	AddProgramVariable("makeChain", &FractalTreeResources::makeChain);
}

void FractalTreeResources::LoadOtherResources()
{
	AddProgramVariable("nearZDistance", &FractalTreeResources::nearZDistance);
	AddProgramVariable("farZDistance", &FractalTreeResources::farZDistance);
	FractalTreeResources::LoadProgramVariables("../Resources/OtherProgramVariables.txt");
}
void FractalTreeResources::LoadResources(RenderingEngine::FractalTreeRenderer* renderer)
{
	BaseResourceDirectory = "../";
	FractalTreeResources::InitializeProgramVariables();
	FractalTreeResources::LoadProgramVariables("../Resources/ProgramVariables.txt");
	//GeneratorDefinition::CreateFromFile("../Resources/Worlds/" + FractalTreeResources::worldName, FractalTreeResources::generatorDefinition);
	FractalTreeResources::ParticleSystemDefinitions[BasicFireTrail] = ParticleSystemDefinition();
	FractalTreeResources::ParticleSystemDefinitions[IneffectiveMissileFireTrail] = ParticleSystemDefinition();
	FractalTreeResources::ParticleSystemDefinitions[DecentMissileFireTrail] = ParticleSystemDefinition();
	FractalTreeResources::ParticleSystemDefinitions[MediumExplosion] = ParticleSystemDefinition();
	FractalTreeResources::ParticleSystemDefinitions[GrenadeFireTrail] = ParticleSystemDefinition();
	FractalTreeResources::ParticleSystemDefinitions[GiantExplosion2] = ParticleSystemDefinition();

	FractalTreeResources::LoadParticleEffect("../Resources/Particle Effects/BasicFireTrail.pe",
		&FractalTreeResources::ParticleSystemDefinitions[BasicFireTrail]);
	FractalTreeResources::LoadParticleEffect("../Resources/Particle Effects/IneffectiveMissileFireTrail.pe",
		&FractalTreeResources::ParticleSystemDefinitions[IneffectiveMissileFireTrail]);
	FractalTreeResources::LoadParticleEffect("../Resources/Particle Effects/DecentMissileFireTrail.pe",
		&FractalTreeResources::ParticleSystemDefinitions[DecentMissileFireTrail]);
	FractalTreeResources::LoadParticleEffect("../Resources/Particle Effects/MediumExplosion.pe",
		&FractalTreeResources::ParticleSystemDefinitions[MediumExplosion]);
	FractalTreeResources::LoadParticleEffect("../Resources/Particle Effects/GrenadeFireTrail.pe",
		&FractalTreeResources::ParticleSystemDefinitions[GrenadeFireTrail]);
	FractalTreeResources::LoadParticleEffect("../Resources/Particle Effects/GiantExplosion2.pe",
		&FractalTreeResources::ParticleSystemDefinitions[GiantExplosion2]);
	FractalTreeResources::LoadParticleEffect("../Resources/Particle Effects/HitExplosion.pe",
		&FractalTreeResources::ParticleSystemDefinitions[HitExplosion]);

	ResourceManager::LoadResourceList(renderer, "../Resources/ResourceList.rl");
}

#endif