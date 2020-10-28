#include "pch.h"
#include "GameResources.h"

#ifndef _NO_GRAPHICS
#include "GameRenderer.h"
#endif
#include <fstream>
#include <sstream>
#include "..\Common Source\Util\File Parser\BMP\FileLoader.h"

using namespace std;

float GameResources::GridWidth = 5;
float GameResources::SquareWidth = 4.5f;
float GameResources::MouseSensitivity = .8f;
bool GameResources::UseInstancing = false;
float GameResources::WorldZDepth = 1;
bool GameResources::renderWireFrame = false;
int GameResources::BMPRegularBlockValue = 255;
int GameResources::BMPHeartBlockValue = 200;
int GameResources::BMPGunBlockValue = 150;
string GameResources::PlayerShipType = "pixelrobot.m";
bool GameResources::LockCamera = false;
bool GameResources::CanDie = true;
float GameResources::StartX = 0;
float GameResources::StartY = 150;
bool GameResources::SpawnEnemies = true;
bool GameResources::UseDiscreteAnimation = true;
bool GameResources::UseDiscreteAnimationPlayer = false;
int GameResources::BMPArmorBlockValue = 100;
int GameResources::RegularBlockStartCount = 20;
int GameResources::ArmorBlockStartCount = 3;
int GameResources::GunBlockStartCount = 2;
int GameResources::ThrusterBlockStartCount = 0;

vector<BlockModel> GameResources::BlockModelDefinitions;
map<string, int> GameResources::BlockModelResourceMap;

vector<RoomModel> GameResources::RoomModelDefinitions;
map<string, int> GameResources::RoomModelResourceMap;

map<string, GameDefinition> GameResources::GameDefinitions;
map<string, int> GameResources::blockTypes;

void GameResources::LoadResources(GameRenderer* renderer)
{
#ifdef _WIN32_APPLICATION
	BaseResourceDirectory = "../App1/Assets/";
#else
	BaseResourceDirectory = "Assets/";
#endif

	GameResources::blockTypes["Regular Block"] = REGULAR;
	GameResources::blockTypes["Armor Block"] = ARMOR;
	GameResources::blockTypes["Gun Block"] = GUN;
	GameResources::blockTypes["Thruster Block"] = THRUSTER;
	GameResources::blockTypes["Heart Block"] = HEART;

	AddProgramVariable("GridWidth", &GameResources::GridWidth);
	AddProgramVariable("SquareWidth", &GameResources::SquareWidth);
	AddProgramVariable("MouseSensitivity", &GameResources::MouseSensitivity);
	AddProgramVariable("UseInstancing", &GameResources::UseInstancing);
	AddProgramVariable("WorldZDepth", &GameResources::WorldZDepth);
	AddProgramVariable("BMPRegularBlockValue", &GameResources::BMPRegularBlockValue);
	AddProgramVariable("BMPHeartBlockValue", &GameResources::BMPHeartBlockValue);
	AddProgramVariable("BMPGunBlockValue", &GameResources::BMPGunBlockValue);
	AddProgramVariable("PlayerShipType", &GameResources::PlayerShipType);
	AddProgramVariable("LockCamera", &GameResources::LockCamera);
	AddProgramVariable("CanDie", &GameResources::CanDie);
	AddProgramVariable("StartX", &GameResources::StartX);
	AddProgramVariable("StartY", &GameResources::StartY);
	AddProgramVariable("SpawnEnemies", &GameResources::SpawnEnemies);
	AddProgramVariable("UseDiscreteAnimation", &GameResources::UseDiscreteAnimation);
	AddProgramVariable("UseDiscreteAnimationPlayer", &GameResources::UseDiscreteAnimationPlayer);
	AddProgramVariable("BMPArmorBlockValue", &GameResources::BMPArmorBlockValue);

	LoadProgramVariables(BaseResourceDirectory + "Resources/ProgramVariables.txt");
	ResourceManager::LoadResourceList(renderer,  BaseResourceDirectory + "Resources/ResourceList.rl");
	loadPixelBlockResources(BaseResourceDirectory + "Resources/PixelBlockResources.rl");
}

void GameResources::loadPixelBlockResources(const string& fileName)
{
	ifstream resourceList(fileName);
	if (resourceList.is_open())
	{
		while (resourceList.good())
		{
			string resourceName;
			string resourceType;

			resourceList >> resourceName;
			resourceList >> resourceType;
			if (resourceType == "model")
			{
				GameResources::loadBlockModel(resourceName);
			}
			else if (resourceType == "room")
			{
				GameResources::loadRoomModel(resourceName);
			}
			else if (resourceType == "gameDefinition")
			{
				GameResources::loadGameDefinition(resourceName);
			}
		}
	}
	else
	{
		exit(23544);
	}

	resourceList.close();
}

void GameResources::loadGameDefinition(const string& resourceName)
{
	map<string, ProgramVar*> loadMap;

	string playerShipType;
	int gunBlockStartCount;
	int armorBlockStartCount;
	int regularBlockStartCount;
	int thrusterBlockStartCount;

	AddProgramVariable("PlayerShipType", &playerShipType, &loadMap);
	AddProgramVariable("GunBlockStartCount", &gunBlockStartCount, &loadMap);
	AddProgramVariable("ArmorBlockStartCount", &armorBlockStartCount, &loadMap);
	AddProgramVariable("RegularBlockStartCount", &regularBlockStartCount, &loadMap);
	AddProgramVariable("ThrusterBlockStartCount", &thrusterBlockStartCount, &loadMap);


	LoadProgramVariables(BaseResourceDirectory + "Resources/Game Definitions/" + resourceName, &loadMap);

	GameResources::GameDefinitions[resourceName] = { playerShipType,
		gunBlockStartCount,
		armorBlockStartCount,
		regularBlockStartCount,
		thrusterBlockStartCount };
}

void GameResources::loadBlockModel(const string& resourceName)
{
	int whichResource = static_cast<int>(BlockModelDefinitions.size());
	BlockModelResourceMap[resourceName] = whichResource;

	ifstream modelDefinition(BaseResourceDirectory + "Resources/blockdefinitions/" + resourceName);

	if (modelDefinition.is_open())
	{
		int** blockArray;
		int fileWidth, fileHeight;
		size_t fileSize;
		string fileName2;
		modelDefinition >> fileName2;
		modelDefinition >> fileName2;

		std::ifstream myFile(GameResources::BaseResourceDirectory + "resources/blockdefinitions/" + fileName2, std::ios::in | std::ios::binary | std::ios::ate);
		if (!myFile.is_open())
		{
			exit(12);
		}
		BlockModelDefinitions.push_back({});
		fileSize = myFile.tellg();
		myFile.seekg(0, std::ios::beg);
		char* blockData = new char[fileSize];
		myFile.read(blockData, fileSize);
		myFile.close();

		blockArray = FileLoader::LoadBMP(blockData, 0, fileWidth, fileHeight);
		delete[] blockData;

		bool foundGun = false;
		for (int a = 0; a < fileWidth; ++a)
		{
			for (int b = 0; b < fileHeight; ++b)
			{
				const int val = blockArray[a][b];
				if (val != 0)
				{
					if (val == GameResources::BMPRegularBlockValue)
					{
						BlockModelDefinitions[whichResource].blockList.push_back({ b - fileWidth / 2, a - fileHeight / 2, BlockTypes::REGULAR, 0, -1, 0, true, b - fileWidth / 2, a - fileHeight / 2, 0, 0, 0 });
					}
					else if (val == GameResources::BMPHeartBlockValue)
					{
						BlockModelDefinitions[whichResource].blockList.push_back({ b - fileWidth / 2, a - fileHeight / 2, BlockTypes::HEART, 0, -1, 0, true, b - fileWidth / 2, a - fileHeight / 2, 0, 0, 0 });
					}
					else if (val == GameResources::BMPGunBlockValue && !foundGun)
					{
						//foundGun = true;
						BlockModelDefinitions[whichResource].blockList.push_back({ b - fileWidth / 2, a - fileHeight / 2, BlockTypes::GUN, 0, -1, 0, true, b - fileWidth / 2, a - fileHeight / 2, 0, 0, 0 });
					}
					else if (val == GameResources::BMPArmorBlockValue)
					{
						BlockModelDefinitions[whichResource].blockList.push_back({ b - fileWidth / 2, a - fileHeight / 2, BlockTypes::ARMOR, 0, -1, 0, true, b - fileWidth / 2, a - fileHeight / 2, 0, 0, 0 });
					}
				}
			}
		}

		for (int a = 0; a < fileWidth; ++a)
		{
			delete[] blockArray[a];
		}

		delete[] blockArray;
	}
}

void GameResources::loadRoomModel(const string& resourceName)
{
	int whichResource = static_cast<int>(GameResources::RoomModelDefinitions.size());
	GameResources::RoomModelResourceMap[resourceName] = whichResource;

	ifstream modelDefinition(BaseResourceDirectory + "Resources/Room Definitions/" + resourceName);

	if (modelDefinition.is_open())
	{
		int** blockArray;
		int fileWidth, fileHeight;
		size_t fileSize;
		string fileName2;
		modelDefinition >> fileName2;
		modelDefinition >> fileName2;

		std::ifstream myFile(GameResources::BaseResourceDirectory + "resources/Room Definitions/" + fileName2, std::ios::in | std::ios::binary | std::ios::ate);
		if (!myFile.is_open())
		{
			exit(12);
		}
		RoomModelDefinitions.push_back({});
		fileSize = myFile.tellg();
		myFile.seekg(0, std::ios::beg);
		char* blockData = new char[fileSize];
		myFile.read(blockData, fileSize);
		myFile.close();

		blockArray = FileLoader::LoadBMP(blockData, 0, fileWidth, fileHeight);
		delete[] blockData;
		RoomModelDefinitions[whichResource].width = fileWidth;
		RoomModelDefinitions[whichResource].height = fileHeight;
		for (int a = 0; a < fileWidth; ++a)
		{
			for (int b = 0; b < fileHeight; ++b)
			{
				const int val = blockArray[a][b];
				if (val != 0)
				{
					if (val == GameResources::BMPRegularBlockValue)
					{
						RoomModelDefinitions[whichResource].blockList.push_back({ b, a, BlockTypes::REGULAR, -1, 0, true });
					}
					else if (val == GameResources::BMPHeartBlockValue)
					{
						RoomModelDefinitions[whichResource].blockList.push_back({ b - fileWidth / 2, a - fileHeight / 2, BlockTypes::HEART, -1, 0, true });
					}
					else if (val == GameResources::BMPGunBlockValue)
					{
						RoomModelDefinitions[whichResource].blockList.push_back({ b - fileWidth / 2, a - fileHeight / 2, BlockTypes::GUN, -1, 0, true });
					}
				}
			}
		}

		for (int a = 0; a < fileWidth; ++a)
		{
			delete[] blockArray[a];
		}

		delete[] blockArray;
	}
}