#include "pch.h"
#include "FractalTreeApplication.h"
#include "FractalTreeWindow.h"
#include "FractalTreeRenderer.h"
#include "UIRenderer.h"
#include "..\Common Source\LSystem\SystemParameters.h"
#include "..\FractalTree Common\FractalTreeSimulation.h"
#include "../FractalTree Common/Game Objects/Objects/TestUserOfSkeleton.h"
#include "../Common Source/Skeleton/Skeleton.h"
#include "../FractalTree Common/Game Objects/Objects/TestChainLink.h"
#include "..\FractalTree Common\Game Objects\Objects\DynamicUserOfSkeleton.h"

using namespace SimulationEngine;
using namespace RenderingEngine;
using namespace SimpleUI;
using namespace DirectX;
using namespace std;
using namespace InverseKinematics;

TreeActor* currentTree = NULL;
using namespace WorldGrowthSimulation;
extern int genQueueCountInd;

void FractalTreeApplication::KeyPressed(short key)
{
	if (!this->window->KeyDown(key))
	{
		if (this->window->SetKeyDown(key))
		{
			return;
		}

		switch (key)
		{
		case '1':
			//++FractalTreeResources::BallScaleFactor;
			TestUserOfSkeleton::ikDistance += 10;
			break;
			//Skeleton::addPi = !Skeleton::addPi;
			this->simulation->theFirstChainLink->ApplyForceAtPoint(this->simulation->theFirstChainLink->location,
				XMVectorSet(10, 0, 10, 0), (float)0);
			break;

		case '2':
			//--FractalTreeResources::BallScaleFactor;
			TestUserOfSkeleton::ikDistance -= 10;
			//Skeleton::useNew = !Skeleton::useNew;
			//FractalTreeResources::useAngleMotor = !FractalTreeResources::useAngleMotor;
			break;

		case '3':
			if (this->simulation == nullptr)
				break;
			//FractalTreeResources::checkVoxels = !FractalTreeResources::checkVoxels;
			//this->simulation->theFirstChainLink->SetPitchYawRollVelocity(XMVectorSet(.1f, 0, 0, 0));
			/*this->simulation->theFirstChainLink->ApplyForceAtPoint(XMVectorAdd(this->simulation->theFirstChainLink->GetLocation(),
				//XMVectorSet(0, -25, 0, 0)), XMVectorSet(rand() % 20, 0, rand()%20, 0), .01f);
				XMVectorSet(0, -25, 0, 0)), XMVectorSet(0, 0, rand() % 20, 0), (float)0);
				//XMVectorSet(0, -25, 0, 0)), XMVectorSet(20, 0, 0, 0));*/
			this->simulation->theFirstChainLink->ApplyForceAtPoint(this->simulation->theFirstChainLink->location,
				XMVectorSet(0, 0, 35, 0), (float)0);
			//this->simulation->theFirstChainLink->velocity = XMVectorAdd(this->simulation->theFirstChainLink->velocity, XMVectorSet(0, 0, 50, 0));
			break;

		case '4':
			//this->simulation->TestLeft();
			//this->simulation->theFirstChainLink->SetPitchYawRollVelocity(XMVectorSet(0, .1f, 0, 0));
			/*this->simulation->theFirstChainLink->ApplyForceAtPoint(XMVectorAdd(this->simulation->theFirstChainLink->GetLocation(),
				//XMVectorSet(0, -25, 0, 0)), XMVectorSet(rand() % 20, 0, rand()%20, 0), .01f);
				XMVectorSet(0, -25, 0, 0)), XMVectorSet(rand() % 20, 0,0, 0), (float)0);*/
			//this->simulation->theFirstChainLink->ApplyTorque(XMVectorSet(0, .05, 0, 0));
			//TestUserOfSkeleton::ikDistance += 10;
			//FractalTreeResources::skeletonRot -= .1f;
			FractalTreeResources::ikEpsilon -= .01f;
			UIRenderer::SetDebugString(yRotInd, "Skeleton Eps (4 / 5): " + std::to_string(FractalTreeResources::ikEpsilon));
			break;

		case '5':
			//this->simulation->TestBump();
			//this->simulation->theFirstChainLink->SetPitchYawRollVelocity(XMVectorSet(0, 0, .1f, 0));
			//Skeleton::useOtherNew = !Skeleton::useOtherNew;
			/*this->simulation->theFirstChainLink->SetVelocity(XMVectorZero());
			this->simulation->theFirstChainLink->SetPitchYawRollVelocity(XMVectorZero());*/
			//UIRenderer::SetDebugString(yRotInd, "Skeleton Rot Y (4 / 5): " + std::to_string(FractalTreeResources::skeletonRot * 180.f / 3.1415f));
			//FractalTreeResources::skeletonRot += .1f;
			//FractalTreeResources::ikEpsilon += .01f;
			FractalTreeResources::resetIK = true;
			UIRenderer::SetDebugString(yRotInd, "Skeleton Eps (4 / 5): " + std::to_string(FractalTreeResources::ikEpsilon));
			break;

		case '6':

		{
			if (this->simulation == nullptr)
				return;
			//this->simulation->TestFoward();
			this->simulation->firstChainLink2->ApplyForceAtPoint(this->simulation->firstChainLink2->location, XMVectorSet(0, 0, FractalTreeResources::testForce, 0));
			break;
		}

		case '7':
			//FractalTreeResources::ikAlpha += .01;
			if (FractalTreeResources::treeUpdatePerFrame > 1)
				--FractalTreeResources::treeUpdatePerFrame;
			UIRenderer::SetDebugString(treeUpdateInd, "Tree update/frame (7 / 8): " + std::to_string(FractalTreeResources::treeUpdatePerFrame));
			//FractalTreeResources::skeletonRot2 -= .31f;
			//UIRenderer::SetDebugString(treeUpdateInd, "Skeleton Rot X (7 / 8): " + std::to_string(FractalTreeResources::skeletonRot2 * 180.f / 3.1415f));
			//UIRenderer::SetDebugString(treeUpdateInd, "Ik Alpha (7 / 8): " + std::to_string(FractalTreeResources::ikAlpha));
			break;
		case '8':
			//FractalTreeResources::ikAlpha -= .01;
			++FractalTreeResources::treeUpdatePerFrame;
			UIRenderer::SetDebugString(treeUpdateInd, "Tree update/frame (7 / 8): " + std::to_string(FractalTreeResources::treeUpdatePerFrame));
			//FractalTreeResources::skeletonRot2 += .31f;
			//UIRenderer::SetDebugString(treeUpdateInd, "Skeleton Rot X (7 / 8): " + std::to_string(FractalTreeResources::skeletonRot2 * 180.f / 3.1415f));
			//UIRenderer::SetDebugString(treeUpdateInd, "Ik Alpha (7 / 8): " + std::to_string(FractalTreeResources::ikAlpha));
			//FractalTreeResources::rotOnce = true;
			break;

		case '0':
			FractalTreeResources::gridCellWidth++;
			UIRenderer::SetDebugString(FractalTreeResources::cellGridWidthIndex, "Cell Grid Width: " + std::to_string(pow(2, FractalTreeResources::gridCellWidth)));
			break;

		case '9':
			/*if (FractalTreeResources::gridCellWidth > 0)
			{
				FractalTreeResources::gridCellWidth--;
				UIRenderer::SetDebugString(FractalTreeResources::cellGridWidthIndex, "Cell Grid Width: " + std::to_string(pow(2, FractalTreeResources::gridCellWidth)));
			}*/
			FractalTreeResources::isPaused = !FractalTreeResources::isPaused;
			break;

		case 'A':
			if (this->simulation == NULL)
				break;

			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeLeft, SimulationEngine::ToggleOn));
			break;

		case 'B':
			UIRenderer::SetDebugString(perlinFreqInd, "Texture Repeat Count (V / B):" + to_string(renderer2->IncreaseTextureRepeatCount()));
			break;

		case 'C':
			StandardRendererResources::CullCells = !StandardRendererResources::CullCells;
			break;

		case 'D':
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeRight, SimulationEngine::ToggleOn));
			break;

		case 'E':
			//FractalTreeResources::growthPaused = !FractalTreeResources::growthPaused;
			Skeleton::desiredDist -= 25;
			break;
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::RollRight, SimulationEngine::ToggleOn));
			break;

		case 'F':
			FractalTreeResources::flyMove = !FractalTreeResources::flyMove;
			break;

		case 'G':
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::Shoot, SimulationEngine::ToggleOn), false);
			break;

		case 'H':
			BaseRendererResources::FastEverything = !BaseRendererResources::FastEverything;
			break;

		case 'I':
			FractalTreeResources::alternateCellOrientation = !FractalTreeResources::alternateCellOrientation;
			break;
		case 'J':
			UIRenderer::SetDebugString(genQueueCountInd, "Generation Queue Size (J / K): " + std::to_string(--FractalTreeResources::cellGenerationCount));
			break;
		case 'K':
			UIRenderer::SetDebugString(genQueueCountInd, "Generation Queue Size (J / K): " + std::to_string(++FractalTreeResources::cellGenerationCount));
			break;
		case 'L':
			FractalTreeResources::UseLOD = !FractalTreeResources::UseLOD;
			break;

		case 'M':
			UIRenderer::SetDebugString(perlinAmpInd, "Fog Density (N / M):" + to_string(renderer2->ChangePerlinAmplitude(.1f)));
			break;

		case 'N':
			UIRenderer::SetDebugString(perlinAmpInd, "Fog Density (N / M):" + to_string(renderer2->ChangePerlinAmplitude(-.1f)));
			break;

		case 'O':
			if (this->simulation == NULL)
				break;
			this->simulation->paused = !this->simulation->paused;
			//this->simulation->ApplyStupidForce();
			break;

		case 'Q':
			InverseKinematics::Skeleton::desiredDist += 25;
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::RollLeft, SimulationEngine::ToggleOn));
			break;

		case 'R':
			//Skeleton::desiredDist += 25;
			//break;
			FractalTreeResources::RenderWireframe = !FractalTreeResources::RenderWireframe;
			this->renderer2->ToggleWireFrame();
			break;

		case 'S':
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeBackward, SimulationEngine::ToggleOn));
			break;

		case 'T':
			TestUserOfSkeleton::fireNext = !TestUserOfSkeleton::fireNext;
			break;
			//FractalTreeResources::renderPrePass = !FractalTreeResources::renderPrePass;
			//StandardRendererResources::renderPrePass = !StandardRendererResources::renderPrePass;

			FractalTreeResources::enableGrowth = !FractalTreeResources::enableGrowth;
			break;

		case 'U':
			FractalTreeResources::displayResourceVoxels = !FractalTreeResources::displayResourceVoxels;
			break;

		case 'V':
			UIRenderer::SetDebugString(perlinFreqInd, "Texture Repeat Count (V / B):" + to_string(renderer2->DecreaseTextureRepeatCount()));
			break;

		case 'W':
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeFoward, SimulationEngine::ToggleOn));
			break;

		case 'X':
			//FractalTreeResources::useLineMeshes = !FractalTreeResources::useLineMeshes;
			FractalTreeResources::constraintIterationCount -= 100;
			UIRenderer::SetDebugString(this->itCountInd, "Constraint Iteration Count (X / Y): " + std::to_string(FractalTreeResources::constraintIterationCount));
			break;
		case 'Y':
			FractalTreeResources::constraintIterationCount += 100;
			UIRenderer::SetDebugString(this->itCountInd, "Constraint Iteration Count (X / Y): " + std::to_string(FractalTreeResources::constraintIterationCount));
			//SystemParameters::UseControlledInput = false;
			break;
		case 'Z':
			//FractalTreeResources::freeMove = !FractalTreeResources::freeMove;
			StandardRendererResources::rotateSun = !StandardRendererResources::rotateSun;
			break;
		case '.':
			if (this->simulation == nullptr) {
				break;
			}

			this->simulation->oneMoreFrame = true;
			break;

		case 27:
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::Jump, SimulationEngine::ToggleOn));
			break;

		}
	}
}

void FractalTreeApplication::KeyReleased(short key)
{
	if (this->window->KeyDown(key))
	{
		switch (key)
		{
		case 'W':
		{
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeFoward, SimulationEngine::ToggleOff));
		}
		break;
		case 'S':
		{
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeBackward, SimulationEngine::ToggleOff));
		}
		break;
		case 'A':
		{
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeLeft, SimulationEngine::ToggleOff));
		}
		break;
		case 'D':
		{
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeRight, SimulationEngine::ToggleOff));
		}
		break;
		case 'E':
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::RollRight, SimulationEngine::ToggleOff), false);
			break;
		case 'G':
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::Shoot, SimulationEngine::ToggleOff), false);
			break;
		case 'Q':
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::RollLeft, SimulationEngine::ToggleOff), false);
			break;
		case 27:
		{
			if (this->simulation == NULL)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::Jump, SimulationEngine::ToggleOff));
		}
		break;
		}
	}

	this->window->SetKeyUp(key);
}

void FractalTreeApplication::MouseMoved(float dx, float dy)
{
	if (this->simulation == NULL)
	{
		this->window->mouseMove(dx, dy);
		return;
	}
	if (!this->window->mouseMove(dx, dy))
		this->sendCommand(Command(Look, 0, dx, dy), true);
}

void FractalTreeApplication::MouseDown(short button, float x, float y)
{
	if (this->window->showMouse) {
		this->window->SetPointerDown(button, x, y);
		return;
	}

	switch (button)
	{
	case 0:
	{
		if (this->simulation == NULL)
			break;
		const XMVECTOR projection = this->GetFloorProjection(x, y);

		const float xx = XMVectorGetX(projection);
		const float zz = XMVectorGetZ(projection);
		const float yy = this->simulation->GetTerrain()->GetHeight(xx, zz);
		const WorldGrowthSimulation::TerrainResources::Voxel* const voxel = this->simulation->GetTerrain()->GetGrowthSimulation()->GetTerrainResources()->GetVoxel(xx, yy, zz);
		if (voxel == nullptr)
		{
			currentTree = new TreeActor(this->simulation->GetTerrain()->GetGrowthSimulation(), ObjectParameters(this->simulation->world), MoveableParameters(), ActorParameters());
			currentTree->location = XMVectorSet(xx, yy, zz, 0);
			//this->simulation->GetTerrain()->GetGrowthSimulation()->GetTerrainResources()->AddVoxel(xx, yy, zz, currentTree->GetTreeSystem()->rootSegment, 1);
		}
	}
	break;
	case 1:
	{
		if (this->simulation == NULL)
			break;
		this->sendCommand(SimulationEngine::Command(SimulationEngine::Shoot2, SimulationEngine::ToggleOn), false);
	}
	break;
	case 2:
	{
		if (this->simulation == NULL)
			break;
		this->sendCommand(SimulationEngine::Command(SimulationEngine::Shoot3, SimulationEngine::ToggleOn), false);
	}
	break;
	}
}

void FractalTreeApplication::MouseUp(short button)
{
	if (this->window->showMouse) {
		this->window->mouseDown[button] = false;
		return;
	}

	switch (button)
	{
	case 1:
		if (this->simulation == NULL)
			break;
		this->sendCommand(SimulationEngine::Command(SimulationEngine::Shoot2, SimulationEngine::ToggleOff), false);
		break;
	case 2:
		if (this->simulation == NULL)
			break;
		this->sendCommand(SimulationEngine::Command(SimulationEngine::Shoot3, SimulationEngine::ToggleOff), false);
		break;
	default:
		break;
	}
}

XMVECTOR FractalTreeApplication::GetFloorProjection(float screenX, float screenY)
{
	const float resX = static_cast<float>(this->renderer2->GetWidth());
	const float resY = static_cast<float>(this->renderer2->GetHeight());
	const float fovAngleYRadians = this->renderer2->GetFovAngleY();
	const float nearZ = this->renderer2->GetNearZPlane();

	const float a = ((2 * screenX) / float(resX) - 1);
	const float b = (1 - (2 * screenY) / float(resY));


	const float c = this->renderer2->GetAspectRatio();
	const float d = 1;

	const float rayDirX = a*c;
	const float rayDirY = b*d;
	const float rayDirZ = -1.0f / tan(fovAngleYRadians / 2);

	// bring the ray into world space
	const XMMATRIX inverse = XMMatrixInverse(nullptr, this->renderer2->GetConstantBufferData().view);

	// for directx conventions... left side multiplication
	const XMMATRIX transpose = XMMatrixTranspose(inverse);


	const XMVECTOR rayDir = XMVectorSet(rayDirX, rayDirY, rayDirZ, 0.0f);
	const XMVECTOR rayDirWorld = DirectX::XMVector4Transform(rayDir, transpose);
	const XMVECTOR eyeInViewSpace = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	const XMVECTOR eyeWorld = DirectX::XMVector4Transform(eyeInViewSpace, transpose);

	const float t = -XMVectorGetY(eyeWorld) / XMVectorGetY(rayDirWorld);
	const float x = t* XMVectorGetX(rayDirWorld) + XMVectorGetX(eyeWorld);
	const float y = 0;
	const float z = t* XMVectorGetZ(rayDirWorld) + XMVectorGetZ(eyeWorld);

	return XMVectorSet(x, y, z, 1.0f);
}