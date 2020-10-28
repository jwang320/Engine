#include "pch.h"
#include "GameSimulation.h"
#include "PlatformerCharacter.h"
#include "PlatformerEnemy.h"
#include "../Simulation Engine Physical/Effect.h"
#include "../Simulation Engine Physical/ForceEffect.h"
#include "PlatformerWorld.h"
#include "PlatformerPersonCharacter.h"
#include "BlockUniverse.h"
#include "GameResources.h"
#include "PlatformerMesquito.h"
#include "PlatformerFodder.h"
#include "PlatformerBlock.h"
#include "MildlyDifficultOpponent.h"
#include "../Src/UIRenderer.h"
#include <string>

using namespace DirectX;
using namespace SimulationEngine;
using std::string;

static int playerLocationIndex = -1;

GameSimulation::GameSimulation(const string& levelName) :
	levelName(levelName)
{

}

void GameSimulation::InitializeWorld()
{
	using namespace SimulationEngine;
	ForceEffect* gravityEffect = new ForceEffect(EffectParameters(this->GetWorld()), ForceEffectParameters(XMVectorSet(0, 0, 0, 0), XMVectorSet(0, -1, 0, 0), .181f));
	this->blockUniverse = new BlockUniverse(512, 512, GameResources::GridWidth, GameResources::SquareWidth, ObjectParameters(this->GetWorld()));
	static_cast<PlatformerWorld*>(this->GetWorld())->SetBlockUniverse(this->blockUniverse);
}

void GameSimulation::InitializeLocalPlayer(int playerID)
{
	if (playerID < 0)
	{
		Player* player = this->GetPlayer(this->CreatePlayer(this->nextAvailableGlobalPlayerID));
		++this->nextAvailableGlobalPlayerID;
		this->localPlayerID = 0;
	}
	else
	{
		this->localPlayerID = playerID;
		Player* player = this->GetPlayer(this->CreatePlayer(playerID));
	}
}

void GameSimulation::EnterWorld()
{
	GameResources::PlayerShipType = GameResources::GameDefinitions[this->levelName].PlayerCharacterType;
	GameResources::RegularBlockStartCount = GameResources::GameDefinitions[this->levelName].RegularBlockStartCount;
	GameResources::ArmorBlockStartCount = GameResources::GameDefinitions[this->levelName].ArmorBlockStartCount;
	GameResources::GunBlockStartCount = GameResources::GameDefinitions[this->levelName].GunBlockStartCount;
	GameResources::ThrusterBlockStartCount = GameResources::GameDefinitions[this->levelName].ThrusterBlockStartCount;

	this->InitializeLocalPlayer();
	if (playerLocationIndex != -1)
	{
		RenderingEngine::UIRenderer::HideDebugString(playerLocationIndex);
	}
	playerLocationIndex = RenderingEngine::UIRenderer::AddDebugString("Player Location: ");
	
	//Create an object of type PlatformerCharacter, and set this object to be Player Zero's focus object.  Commands given by Player Zero will
	//be routed to the player's focus object.
	PlatformerPersonCharacter* platformerChar = new PlatformerPersonCharacter(this->blockUniverse, -1, 0, ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(GameResources::StartX, GameResources::StartY, 0, 0)));
	this->playerChar = platformerChar;
	this->playerList[0]->SetFocusObject(platformerChar->GetGlobalID());

		
	if (GameResources::SpawnEnemies)
	{
		//Create an enemy
		/*PlatformerEnemy* platformerEnemy = new PlatformerEnemy(this->blockUniverse, -1, 1, ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(1150, 500, 0, 0)), platformerChar);
		platformerEnemy->InitializeAIControl(PlatformerBrain::AIBrainParameters(400, 50, 200, 700));
		

		PlatformerMesquito* platformerEnemy2 = new PlatformerMesquito(this->blockUniverse, -1, 1, ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(600, 1500, 0, 0)), platformerChar);
		platformerEnemy2->InitializeAIControl();
		
		PlatformerMesquito* platformerEnemy3 = new PlatformerMesquito(this->blockUniverse, -1, 1, ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(850, 500, 0, 0)), platformerChar);
		platformerEnemy3->InitializeAIControl(PlatformerBrain::AIBrainParameters(300, 50, 200, 400, PlatformerBrain::CHASE));
		
		PlatformerFodder* platformerEnemy4 = new PlatformerFodder(this->blockUniverse, -1, 1, ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(800, 1500, 0, 0)), platformerChar);
		platformerEnemy4->InitializeAIControl();
		
		PlatformerFodder* platformerEnemy5 = new PlatformerFodder(this->blockUniverse, -1, 1, ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(900, 500, 0, 0)), platformerChar);
		platformerEnemy5->InitializeAIControl();*/
		
		/*PlatformerEnemy* platformerEnemy6 = new PlatformerEnemy(this->blockUniverse, -1, 1, ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(1300, 500, 0, 0)), platformerChar);
		platformerEnemy6->InitializeAIControl();
		
		PlatformerEnemy* platformerEnemy7 = new PlatformerEnemy(this->blockUniverse, -1, 1, ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(1600, 500, 0, 0)), platformerChar);
		platformerEnemy7->InitializeAIControl();*/
		
		/*MildlyDifficultOpponent* platformerEnemy8 = new MildlyDifficultOpponent(this->blockUniverse, -1, 1, ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(200, 4000, 0, 0)), platformerChar);
		platformerEnemy8->InitializeAIControl(PlatformerBrain::AIBrainParameters(700, 50, 1300));*/
	}
}

void GameSimulation::ResetWorld()
{
	if (this->world != NULL)
	{
		delete this->world;
	}

	this->world = new PlatformerWorld();
}

bool GameSimulation::TestCreateBlock(const XMVECTOR& pos, const string& blockType)
{
	this->playerChar->ResetAnimationState();
	if (this->playerChar->AddBlock(pos, blockType))
	{
		return true;
	}

	return false;
}

void GameSimulation::Update()
{
	__super::Update();
	XMFLOAT4 tempLoc;
	XMStoreFloat4(&tempLoc, this->playerChar->GetLocation());
	std::string locString = std::to_string(static_cast<int>(tempLoc.x)) + ", " + std::to_string(static_cast<int>(tempLoc.y)) + ", " + std::to_string(static_cast<int>(tempLoc.z));
	RenderingEngine::UIRenderer::SetDebugString(playerLocationIndex, "Player Location: " + locString);
}

int GameSimulation::GetPlayerCharacterID() const
{
	if (this->playerChar != NULL)
	{
		return this->playerChar->GetGlobalID();
	}

	return -1;
}