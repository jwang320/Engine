#pragma once

#pragma once
//#pragma warning(1: 4711)
#include "FractalTreeResources.h"

#define USE_RENDER_THREAD

enum class ObjectTypes : unsigned short
{
	Ship,
	Humanoid,
	Terrain,
	TerrainCell,
	Projectile,
	FoliageActor,
	TreeActor,
	TheSun,
	TestFloor,
	TestShip,
	DecentMissile,
	IneffectiveMissile,
	BadGuy,
	GoodGuy,
	WebHouse,
	SkeletonMesh,
	Teapot,
	Forest
};

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
#include "Allocate.h"
#include "SystemParameters.h"
#include "..\Simulation Engine Physical\Mesh.h"

#include "..\Simulation Engine Physical\Moveable.h"
#include "..\Simulation Engine Physical\Commandable.h"
#include "..\Simulation Engine Physical\Actor.h"

#include "..\FractalTree32\FractalTreeWorld.h"
#include "..\Simulation Engine Physical\PhysicalWorldSimulation.h"
#include "BoundingBox.h"
#include "PerlinNoise.h"

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