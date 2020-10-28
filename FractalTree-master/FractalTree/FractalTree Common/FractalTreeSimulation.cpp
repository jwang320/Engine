#include "pch.h"
#include "FractalTreeSimulation.h"
#ifndef _NO_GRAPHICS
using namespace RenderingEngine;
#include "UIRenderer.h"
#endif
#include "../FractalTree Common/Game Objects/Objects/GoodGuy.h"
#include "../FractalTree Common/Game Objects/Objects/Humanoid.h"
#include "Game Objects/Objects/TestUserOfSkeleton.h"
#include "Game Objects/Objects/TestChainLink.h"
#include "Game Objects/Objects/Projectile.h"
#include "../Common Source/Skeleton/Skeleton.h"
#include "../Common Source/Skeleton/SkeletonSegment.h"
#include "Game Objects/Objects/DynamicUserOfSkeleton.h"
using namespace ProceduralGeneration;
using namespace Geometry;
using namespace DirectX;
using namespace std;
float camHeight = 0;

namespace SimulationEngine
{
	FractalTreeSimulation::FractalTreeSimulation() :
		floor(NULL),
		localPlayerID(-1),
		testBadGuy(NULL)
	{

	}

	void FractalTreeSimulation::Initialize()
	{
		__super::Initialize();
		this->cellGridWidth = powf(2, static_cast<float>(FractalTreeResources::gridCellWidth));
		this->cellWidth = static_cast<int>(FractalTreeResources::cellSize);
		//ForceEffect* gravityEffect = new ForceEffect(this->GetWorld(), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, -1, 0, 0), .181f);
		
#ifndef _NO_GRAPHICS
		FractalTreeResources::PlayerLocDebugIndex = UIRenderer::AddDebugString("Player Location: ");
#endif

	}

	void FractalTreeSimulation::ResetWorld()
	{
		if(this->world != NULL)
		{
			delete this->world;
		}

		this->world = new FractalTreeWorld();
	}

	void FractalTreeSimulation::InitializeLocalPlayer(int playerID)
	{
		if(playerID < 0)
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

	void FractalTreeSimulation::InitializeWorld(bool generateTerrain, bool generateAnything)
	{
		this->generateTerrain = generateTerrain;
		if (!FractalTreeResources::useShip && FractalTreeResources::useGravity)
		{
			ForceEffect* const gravityEffect = new ForceEffect(EffectParameters(this->GetWorld()), ForceEffectParameters(XMVectorSet(0, 0, 0, 0), XMVectorSet(0, -1, 0, 0), .181f));
		}
		new TheSun(ObjectParameters(this->world));
		if(this->generateTerrain)
		{
			FractalTreeResources::PlayerCellDebugIndex = UIRenderer::AddDebugString("Current Cell: ");
			Terrain* const testTerrain = new Terrain(ObjectParameters(this->GetWorld()), this->cellWidth, this->cellGridWidth);
			this->terrain = testTerrain;
			this->GetWorld()->SetTerrain(testTerrain);
		}
		else
		{
			if(generateAnything)
			{
				FractalTreeResources::flyMove = false;
				this->floor = new TestFloor(ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(0, 0, 0, 0)));
			}
		}
	}

	void FractalTreeSimulation::Update()
	{
		__super::Update();
#ifndef _NO_GRAPHICS
		if(this->PlayerExists(this->localPlayerID))
		{
			if(this->DoesObjectExist(this->GetPlayer(this->localPlayerID)->GetFocusObject()))
			{
				XMFLOAT4 tempLoc;
				XMStoreFloat4(&tempLoc, this->GetWorld()->GetActor(this->playerList[0]->GetFocusObject())->location);
				std::string locString =  std::to_string(static_cast<int>(tempLoc.x)) + ", " + std::to_string(static_cast<int>(tempLoc.y)) + ", " + std::to_string(static_cast<int>(tempLoc.z));
				RenderingEngine::UIRenderer::SetDebugString(FractalTreeResources::PlayerLocDebugIndex, "Player Location: " + locString);

				if (this->generateTerrain)
				{
					const int cellX = static_cast<int>((tempLoc.x) / this->cellWidth);
					const int cellZ = static_cast<int>((tempLoc.z) / this->cellWidth);
					std::string cellString = std::to_string(cellX) + ", " + std::to_string(cellZ);
					RenderingEngine::UIRenderer::SetDebugString(FractalTreeResources::PlayerCellDebugIndex, "Current Cell: " + cellString);
				}
			}
		}
#endif
	}

	void FractalTreeSimulation::ClearWorld()
	{
		if(this->PlayerExists(0))
		{
			if(this->playerList[0]->GetFocusObject() == this->world->GetObjectGlobalID(this->world->GetObjectCount() - 1))
			{
				return;
			}
		}

		this->world->RemoveObject(this->world->GetObjectGlobalID(this->world->GetObjectCount() - 1));
	}

	void FractalTreeSimulation::EnterWorld(bool generateTerrain)
	{
		this->InitializeLocalPlayer();
		if(FractalTreeResources::useShip)
		{
			camHeight = 0;
			GoodGuy* testHumanoid = new GoodGuy(ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(this->generateTerrain ? FractalTreeResources::startX : 0, 0, this->generateTerrain ? FractalTreeResources::startZ : 0, 0)));
			this->playerList[0]->SetFocusObject(testHumanoid->GetGlobalID());
		}
		else
		{
			camHeight = 35;
			Humanoid* testHumanoid = new Humanoid(ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(this->generateTerrain ? FractalTreeResources::startX : 190, 0, this->generateTerrain ? FractalTreeResources::startZ : -450, 0))); //i really shouldn't change this
			this->playerList[0]->SetFocusObject(testHumanoid->GetGlobalID());
			Actor* testHumanoid2;
			if (FractalTreeResources::dynamicIk)
			{
				testHumanoid2 = new DynamicUserOfSkeleton(ObjectParameters(this->GetWorld()),
					MoveableParameters(XMVectorAdd(XMVectorSet(-100, 50 /*+ FractalTreeResources::skeletonTestSegmentCount * 50*/, 50, 0), XMVectorSet(this->generateTerrain ? FractalTreeResources::startX : 190, 0, this->generateTerrain ? FractalTreeResources::startZ : -450, 0)))); //i really shouldn't change this
			}
			else
			{
				testHumanoid2 = new TestUserOfSkeleton(ObjectParameters(this->GetWorld()),
					MoveableParameters(XMVectorAdd(XMVectorSet(-100, 50 /*+ FractalTreeResources::skeletonTestSegmentCount * 50*/, 150, 0), XMVectorSet(this->generateTerrain ? FractalTreeResources::startX : 190, 0, this->generateTerrain ? FractalTreeResources::startZ : -450, 0)))); //i really shouldn't change this
			}
			
			this->GetWorld()->testArm = testHumanoid2;
			//this->theFirstChainLink = testHumanoid2->GetSkeleton()->GetSegments().back()->attachedObject;
			if (this->PlayerExists(this->localPlayerID))
			{
				testHumanoid2->TrackTarget(this->GetPlayer(this->localPlayerID)->GetFocusObject());
			}

			const float mass = 25;
			const float inertia = 1;
			if (FractalTreeResources::makeChain)
			{
				const float len = 25;
				this->firstChainLink2 = new TestChainLink(ObjectParameters(this->GetWorld()), len,
					MoveableParameters(XMVectorAdd(XMVectorSet(-100, 150, 400, 0), XMVectorSet(this->generateTerrain ? FractalTreeResources::startX : 290, 0, this->generateTerrain ? FractalTreeResources::startZ : -650, 0)),
						XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 0, 0, 0))); //i really shouldn't change this
																									 //this->GetWorld()->AddConstraint(theFirstChainLink, XMVectorAdd(theFirstChainLink->GetLocation(), XMVectorSet(40, 125, 0, 0)), XMVectorSet(0, 19, 0, 0));
																									 //theFirstChainLink->SetVelocity(XMVectorSet(1, 0, 0, 0));
																									 //theFirstChainLink->SetPitchYawRoll2(XMVectorSet(0, DirectX::XM_PI, 0, 0));
																									 //theFirstChainLink->SetPitchYawRollVelocity(XMVectorSet(0, 0, 0, 0));
																									 //theFirstChainLink->ApplyForceAtPoint(XMVectorAdd(theFirstChainLink->GetLocation(), XMVectorSet(0, -25, 10, 0)), XMVectorSet(.001f, 0, 0, 0));
				const XMVECTOR offset = XMVectorSet(0, len/2, 0, 0);//XMVector3Transform(XMVectorSet(0, 25, 0, 0), XMMatrixRotationRollPitchYawFromVector(theFirstChainLink->GetPitchYawRoll()));
				this->GetWorld()->AddConstraint(firstChainLink2, XMVectorAdd(firstChainLink2->location, offset), XMVectorSet(0, len/2, 0, 0));
				this->firstChainLink2->SetMass(mass);
				this->firstChainLink2->rotationalInertia = inertia;
				/*TestChainLink* const testChainLink2 = new TestChainLink(ObjectParameters(this->GetWorld()),
				MoveableParameters(XMVectorAdd(XMVectorSet(0, -50, 0, 0), theFirstChainLink->GetLocation()))); //i really shouldn't change this
				this->GetWorld()->AddConstraint(theFirstChainLink, testChainLink2, XMVectorSet(0, -25, 0, 0), XMVectorSet(0, 25, 0, 0));
				testChainLink2->SetVelocity(XMVectorSet(1, 0, 0, 0));
				TestChainLink* const testChainLink3 = new TestChainLink(ObjectParameters(this->GetWorld()),
				MoveableParameters(XMVectorAdd(XMVectorSet(0, -50, 0, 0), testChainLink2->GetLocation()))); //i really shouldn't change this
				this->GetWorld()->AddConstraint(testChainLink2, testChainLink3, XMVectorSet(0, -25, 0, 0), XMVectorSet(0, 25, 0, 0));

				TestChainLink* const testChainLink4 = new TestChainLink(ObjectParameters(this->GetWorld()),
				MoveableParameters(XMVectorAdd(XMVectorSet(0, -50, 0, 0), testChainLink3->GetLocation()))); //i really shouldn't change this
				this->GetWorld()->AddConstraint(testChainLink3, testChainLink4, XMVectorSet(0, -25, 0, 0), XMVectorSet(0, 25, 0, 0));
				testChainLink4->SetVelocity(XMVectorSet(10, 0, 10, 0));*/

				//this->GetWorld()->AddAngleConstraint(this->firstChainLink2, testHumanoid2, XMVectorSet(0, len, 0, 0), XMVectorSet(0, -len/2, 0, 0), true);
				TestChainLink* previousLink = this->firstChainLink2;
				for (int a = 0; a < FractalTreeResources::skeletonTestSegmentCount; ++a)
				{
					const float ang = .35f;
					TestChainLink* const testChainLink2 = new TestChainLink(ObjectParameters(this->GetWorld()), len,
						MoveableParameters(XMVectorAdd(XMVectorSet(0, -len, 0, 0), previousLink->location))); //i really shouldn't change this
					testChainLink2->constraintTargetTest = XMQuaternionRotationRollPitchYaw(0, 0, rand() % 2 == 0 ? ang : ang);
					this->GetWorld()->AddConstraint(previousLink, testChainLink2, XMVectorSet(0, -len / 2, 0, 0), XMVectorSet(0, len / 2, 0, 0));
					//this->GetWorld()->AddAngleConstraint(testChainLink2, previousLink, XMVectorSet(0, len / 2, 0, 0), XMVectorSet(0, -len / 2, 0, 0));
					previousLink = testChainLink2;
					testChainLink2->SetMass(mass);
					testChainLink2->rotationalInertia = inertia;
				}
				this->firstChainLink2 = previousLink;
				this->firstChainLink2->SetMass(5);
				this->firstChainLink2->rotationalInertia = 1;
				//this->firstChainLink2->SetVelocity(XMVectorSet(0, 0, 55, 0));
			}
		}
	}

	void FractalTreeSimulation::TestLeft()
	{
		if(this->testBadGuy == NULL)
			return;

		this->testBadGuy->TurnLeft(1);
	}

	void FractalTreeSimulation::TestFoward()
	{
		const float startX =FractalTreeResources::startX + 1000;
		const float startZ = FractalTreeResources::startZ;
		//this->testBadGuy->GoFoward();
		BadGuy* b = new BadGuy(ObjectParameters(this->GetWorld()), MoveableParameters(XMVectorSet(startX, 50, startZ, 0)));
		this->testBadGuy = b;
		if (this->PlayerExists(this->localPlayerID))
		{
			b->TrackTarget(this->GetPlayer(this->localPlayerID)->GetFocusObject());
		}
	}

	void FractalTreeSimulation::TestBump()
	{
		if(this->testBadGuy == NULL)
			return;

		this->testBadGuy->Bump();
	}

	void FractalTreeSimulation::ApplyStupidForce()
	{
		if(this->testBadGuy == NULL)
			return;

		if(this->PlayerExists(this->localPlayerID) && this->testBadGuy != NULL)
		{
			this->testBadGuy->Hover(this->GetPlayer(this->localPlayerID)->GetFocusObject());
		}
	}
}