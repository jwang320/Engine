// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef PCH_H
#define PCH_H

// TODO: add headers that you want to pre-compile here
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
	DynamicSkeleton
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
#include <DirectXMath.h>
#include "..\Common Source\Eigen\dense"
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <thread>
#include <sstream>
#include <mutex>
#include <queue>
#include <unordered_set>
#include "../Common Source/Util/MathUtil.h"
#include "../Common Source/Simulation Engine/Base Simulation/Object.h"
#include "../Common Source/Simulation Engine/Base Simulation/World.h"
#include "../Common Source/Simulation Engine/Base Simulation/Simulation.h"

#include "..\Common Source\Simulation Engine\Physical Simulation\TestMat.h"
#include "../Common Source/Simulation Engine/Physical Simulation/Interfaces/Moveable.h"
#include "../Common Source/Simulation Engine/Physical Simulation/Actor.h"
#include "../Common Source/Simulation Engine/Physical Simulation/PhysicalWorld.h"
#include "..\Common Source\Simulation Engine\Physical Simulation\Constraint.h"
#include "../Common Source/Simulation Engine/Physical Simulation/Effects/Effect.h"
#include "../Common Source/Simulation Engine/Physical Simulation/Effects/ForceEffect.h"
#endif //PCH_H
