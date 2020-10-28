#pragma once
#define _APP1
#ifndef _WIN32_APPLICATION
#include <wrl.h>
#include <wrl/client.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1effects_1.h>
#include <dwrite_2.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <memory>
#include <agile.h>
#include <concrt.h>
#else
#define _USE_RENDER_THREAD
#endif

enum class ObjectTypes : unsigned short
{
	NOT_AN_OBJECT,
	PlatformerCharacter,
	PlatformerEnemy,
	BlockUniverse,
	PlatformerPersonCharacter,
	PlatformerMissile,
	PlatformerBlock
};


#include <vector>
#include <DirectXMath.h>
#include "..\src\BoundingBox.h"

#include "..\Simulation Engine Physical\Moveable.h"
#include "..\Simulation Engine Physical\Commandable.h"
#include "..\Simulation Engine Physical\Actor.h"

#include "..\Simulation Engine Physical\PhysicalWorld.h"
#include "..\Simulation Engine Physical\PhysicalWorldSimulation.h"
//#include "..\src\BoundingBox.h"
#ifndef _NO_GRAPHICS
#include "../Rendering Engine/Direct3DRenderer.h"
#include "../Rendering Engine/Direct3DBase.h"
/*
#include "Window.h"
#include "Rendering Engine/Renderer/Renderer Base/Direct3DRenderer.h"
#include "Rendering Engine/Direct3DBase.h"
#include "ObjectRendererBinding.h"
#include "Rendering Engine/Scene Graph/SceneGraph.h"
#include "Rendering Engine/ASCIIModelParser.h"
#include "Rendering Engine/Animation/Animation.h"
#include "Rendering Engine/Animation/AnimationState.h"
#include "Rendering Engine/Animation/AnimationTransform.h"
#include "Rendering Engine/Scene Graph/SceneNode.h"
#include "Rendering Engine/Scene Graph/SceneEntity.h"
#include "Rendering Engine/Scene Graph/ParticleEntity.h"
#include "RendererResources.h"*/
#endif

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