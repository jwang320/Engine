#pragma once
#ifndef _RTS_GAME
//#pragma warning(1: 4711)
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <thread>
#include <sstream>
#include <mutex>
#include <queue>
#include <unordered_set>
#include "FractalTreeResources.h"

#define USE_RENDER_THREAD

#ifndef _MADE_THIS_
#define _MADE_THIS_
enum class ObjectTypes : unsigned short
{
	NOT_AN_OBJECT,
	Humanoid,
	Terrain,
	TerrainCell,
	Projectile,
	TreeActor,
	TheSun,
	TestFloor,
	DecentMissile,
	IneffectiveMissile,
	BadGuy,
	GoodGuy,
	TestUserOfSkeleton,
	TestChainLink,
	TestSkeletonArmSegment,
	DynamicSkeleton,
	SpaceBall
};
#endif

enum class ProgramMessageType : char
{
	DestroyObject,
	UpdateObject,
	AssignPlayerObject,
	UserInput,
	WorldEvent,
	RequestObject,
	CreateObject
};

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define PACKET_SIZE 1024
#define OBJECT_QUEUED_ADD -5

#ifdef _WIN32_APPLICATION
#define WIN32_LEAN_AND_MEAN 
#define DIRECTINPUT_VERSION 0x0800
#include <time.h>
#include <wtypes.h>
#include <d3d11.h>
#endif
#include <DirectXMath.h>
//#include "..\Common Source\Eigen\Core"

#include "BoundingBox.h"
#ifndef _NO_GRAPHICS
#endif

#define MAXIMUM_VERTEX_COUNT 500000
#define MAXIMUM_INDEX_COUNT 100000
static const bool TEST_VERSION = false;


#define KEY_ENTER 13
#define KEY_DOWNARROW 40
#define KEY_UPARROW 38

#define KEY_COUNT 39
#define BUTTON_COUNT 2
#define KEY_A 1
#define KEY_B 2
#define KEY_C 3
#define KEY_D 4
#define KEY_E 5
#define KEY_F 6
#define KEY_G 7
#define KEY_H 8
#define KEY_I 9
#define KEY_J 10
#define KEY_K 11
#define KEY_L 12
#define KEY_M 13
#define KEY_N 14
#define KEY_O 15
#define KEY_P 16
#define KEY_Q 17
#define KEY_R 18
#define KEY_S 19
#define KEY_T 20
#define KEY_U 21
#define KEY_V 22
#define KEY_W 23
#define KEY_X 24
#define KEY_Y 25
#define KEY_Z 26
#define KEY_SPACE 27
#define KEY_ESCAPE 28
#define KEY_RETURN 29
#define KEY_DOWN 30
#define KEY_UP 31
#define KEY_LEFT 32
#define KEY_RIGHT 33
#define KEY_TAB 34
#define KEY_BACKSPACE 35
#define KEY_1 36
#define KEY_2 37
#define KEY_3 38
#define KEY_4 39
#define KEY_5 40
#define KEY_6 41
#define KEY_7 42
#define KEY_8 43
#define KEY_9 44
#define KEY_0 45
#define KEY_PERIOD 46

void FailExit();

//#include "..\Common Source\Skeleton\SkeletonSegment.h"
//#include "..\Common Source\Skeleton\Skeleton.h"

#include "../Common Source/Util/MathUtil.h"
#include "../Common Source/Simulation Engine/Base Simulation/Object.h"
#include "../Common Source/Simulation Engine/Base Simulation/World.h"
#include "../Common Source/Simulation Engine/Base Simulation/Simulation.h"

#include "..\Common Source\Simulation Engine\Physical Simulation\TestMat.h"
#include "../Common Source/Simulation Engine/Physical Simulation/Interfaces/Moveable.h"
#include "../Common Source/Simulation Engine/Physical Simulation/Actor.h"
#include "../Common Source/Simulation Engine/Physical Simulation/PhysicalWorld.h"
#include "..\Common Source\Simulation Engine\Physical Simulation\Constraint.h"
//#include "..\Common Source\Simulation Engine\Physical Simulation\AngleConstraint.h"
#include "../Common Source/Simulation Engine/Physical Simulation/Effects/Effect.h"
#include "../Common Source/Simulation Engine/Physical Simulation/Effects/ForceEffect.h"

#include "../Common Source/Rendering Engine/Rendering Engine Base/Direct3DBase.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Base\Direct3DRenderer.h"
#include "../Common Source/Rendering Engine/Rendering Engine Base/BaseRendererResources.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\SceneNode.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\StandardCubeRenderer.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\SceneEntity.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\ParticleEntity.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\ParticleNode.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\SceneGraph.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\ParticleSystem.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\ModelVertexTypes.h"
#include "../Common Source/Rendering Engine/Rendering Engine Standard/StandardRendererResources.h"
#include "../Common Source/Rendering Engine/Rendering Engine Standard/StandardCubeRenderer.h"

#include "../Common Source/Terrain Simulation/GrowthSimulation.h"
#include "../Common Source/Terrain Simulation/TerrainResources.h"

#include "../FractalTree Common/Game Objects/Objects/TestMissileLauncher.h"
#include "../FractalTree Common/Game Objects/Objects/DecentMissileLauncher.h"
#include "../FractalTree Common/Game Objects/Objects/IneffectiveMissileLauncher.h"
#include "../FractalTree Common/Game Objects/Objects/BadGuy.h"
#include "../FractalTree Common/Game Objects/Objects/Terrain.h"
#include "../FractalTree Common/Game Objects/Objects/TheSun.h"
#include "../FractalTree Common/Game Objects/Objects/TestFloor.h"
#include "../FractalTree Common/Game Objects/Objects/TreeActor.h"
#include "..\FractalTree Common\Game Objects\Objects\TestSkeletonArmSegment.h"
#include "..\FractalTree Common\Game Objects\Objects\TestUserOfSkeleton.h"
#include "..\FractalTree Common\Game Objects\Objects\TestChainLink.h"
#include "../FractalTree Common/Game Objects/Objects/DecentMissile.h"

#include "..\Common Source\LSystem\SystemParameters.h"
#include "..\Common Source\LSystem\SpatialLSystem.h"

#include "..\Common Source\Mesh\HeightMapMesh.h"
#include "..\Common Source\Mesh\Geometry.h"


std::string getVectorString(const DirectX::XMVECTOR& v);
std::string getVectorStringDeg(const DirectX::XMVECTOR& v);
float getVectorLen(const DirectX::XMVECTOR& v);
#endif
