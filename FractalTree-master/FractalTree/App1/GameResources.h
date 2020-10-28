#pragma once
#include "..\src\ResourceManager.h"

#include "PlatformerCharacter.h"
#include "GameDefinition.h"

class GameRenderer;

struct BlockModel
{
	std::vector<BlockCoordinate> blockList;
};

struct RoomModel
{
	std::vector<BlockCoordinate> blockList;
	int width;
	int height;
};

class GameResources : public ResourceManager
{
public:
#ifndef _NO_GRAPHICS
	enum ParticleSystems
	{
		BasicFireTrail,
		IneffectiveMissileFireTrail,
		DecentMissileFireTrail,
		GiantExplosion,
		MediumExplosion,
		HugeExplosion,
		GrenadeFireTrail,
		GiantExplosion2
	};
#endif

public:
	static void LoadResources(GameRenderer* renderer = NULL);

	static float GridWidth;
	static float SquareWidth;
	static float MouseSensitivity;
	static bool UseInstancing;
	static float WorldZDepth;
	static bool renderWireFrame;
	static int BMPRegularBlockValue;
	static int BMPHeartBlockValue;
	static int BMPGunBlockValue;
	static std::string PlayerShipType;
	static bool LockCamera;
	static bool CanDie;
	static float StartX;
	static float StartY;
	static bool SpawnEnemies;
	static bool UseDiscreteAnimation;
	static bool UseDiscreteAnimationPlayer;
	static int BMPArmorBlockValue;
	static int RegularBlockStartCount;
	static int GunBlockStartCount;
	static int ArmorBlockStartCount;
	static int ThrusterBlockStartCount;

	static std::vector<BlockModel> BlockModelDefinitions;
	static std::map<std::string, int> BlockModelResourceMap;

	static std::vector<RoomModel> RoomModelDefinitions;
	static std::map<std::string, int> RoomModelResourceMap;

	static std::map<std::string, GameDefinition> GameDefinitions;

	static std::map<std::string, int> blockTypes;
	//static std::map<std::string, int> BlockTypeIndexMap;
private:
	static void loadPixelBlockResources(const std::string& fileName);
	static void loadGameDefinition(const std::string& resourceName);
	static void loadBlockModel(const std::string& resourceName);
	static void loadRoomModel(const std::string& resourceName);
};