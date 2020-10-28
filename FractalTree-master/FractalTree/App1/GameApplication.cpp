#include "pch.h"
#include "GameApplication.h"
#include "GameRenderer.h"
#include "GameMenuWindow.h"
#include "GameBindingGraph.h"
#include "GameResources.h"
#include "..\Rendering Engine Standard\SceneGraph.h"
#include "../rendering engine/BaseRendererResources.h"
#include <string>
#ifndef _WIN32_APPLICATION
#include <agile.h>
#else
#include <thread>
using std::thread;
#endif
#include "..\src\UIRenderer.h"

using namespace Win8GameRenderingEngine;
using namespace DirectX;
using std::string;

extern double performanceFrequency;
static int simulationTimeIndex;
static int renderTimeIndex;
bool GameApplication::running = true;
volatile bool GameApplication::finishedRendering = false;

GameApplication::GameApplication() :
	gameSimulation(NULL),
	window(NULL),
	bindingGraph(NULL),
	renderer(NULL),
	playerID(0),
	cameraVelocity(XMVectorSet(0, 0, 0, 0)),
	cameraSpeed(10),
	cursorLocation(XMFLOAT4(0, 0, 0, 0)),
	cursorActivated(false),
	hasReset(true)
{

}

void GameApplication::Initialize()
{
	this->renderer = new GameRenderer();
	this->window = new GameMenuWindow(this);
	this->renderer->SetWindow(this->window);
	renderTimeIndex = RenderingEngine::UIRenderer::AddDebugString("Render Time: ");
	simulationTimeIndex = RenderingEngine::UIRenderer::AddDebugString("Simulation Time: ");
}

//InitializeSimulation is called when the player enters a new game (typically by selecting
//the "Enter Game World" option in the game menu.
void GameApplication::InitializeSimulation(const string& levelName, bool network, int worldType, int clientID)
{
	this->hasReset = true;
	bool restartRenderer = false;

	if (this->bindingGraph != NULL)
	{

#ifdef USE_RENDER_THREAD
		running = false;
		//wait for the renderer thread to terminate before deleting the binding graph
		while (!finishedRendering)
		{
		}
		restartRenderer = true;
#endif
		delete this->bindingGraph;
	}
	if (this->gameSimulation != NULL)
	{
		delete this->gameSimulation;
	}

	this->gameSimulation = new GameSimulation(levelName);
	this->gameSimulation->Initialize();
	this->window->SetSimulation(this->gameSimulation);
	this->renderer->GetSceneGraph()->Reset();
	this->bindingGraph = new GameBindingGraph(this->gameSimulation, this->renderer->GetSceneGraph(), this->renderer);

	this->gameSimulation->InitializeWorld();
	this->gameSimulation->EnterWorld();

	this->gameSimulation->SetLocalPlayerID(this->playerID);
	this->bindingGraph->SetClientID(this->playerID);

#ifdef USE_RENDER_THREAD
	if (restartRenderer)
	{
		running = true;
		thread renderThread(&GameApplication::RenderThread, this);
		renderThread.detach();
	}
#endif
}

//This function is called once per frame.
void GameApplication::Update()
{
	//Only update the game simulation if the player has entered a game.
	if (this->gameSimulation != NULL)
	{
		//Update the simulation, and time how long it takes to update the frame.
		_LARGE_INTEGER startTime; QueryPerformanceCounter(&startTime);

		this->gameSimulation->Update();

		_LARGE_INTEGER simulationEndTime; QueryPerformanceCounter(&simulationEndTime);
		RenderingEngine::UIRenderer::SetDebugString(simulationTimeIndex, "Simulation Time: " + std::to_string((simulationEndTime.QuadPart - startTime.QuadPart) / performanceFrequency));
	}

	//Update the renderer after updating the simulation.  If the user is running in multi-threaded mode,
	//this function is called elsewhere asynchronously.
#ifndef USE_RENDER_THREAD
	this->updateRenderer();
#endif
}

void GameApplication::updateRenderer()
{
	_LARGE_INTEGER startTime;
	QueryPerformanceCounter(&startTime);


	this->renderer->GetSceneGraph()->SetCameraLocation(XMVectorAdd(this->renderer->GetSceneGraph()->GetCameraLocation(), this->cameraVelocity));

	//Calling Update() on the BindingGraph will link the current game simulation state
	//to the graphics' scene graph so the renderer can draw it to the screen.
	if (this->bindingGraph != NULL)
		this->bindingGraph->Update(); 

	this->renderer->Render();
	this->renderer->Present();

	_LARGE_INTEGER renderEndTime;
	QueryPerformanceCounter(&renderEndTime);

	const std::string renderString = std::to_string((renderEndTime.QuadPart - startTime.QuadPart) / performanceFrequency);
	RenderingEngine::UIRenderer::SetDebugString(renderTimeIndex, "Render Time: " + renderString);
}

#ifndef _WIN32_APPLICATION 
void GameApplication::InitializeRenderer(Windows::UI::Core::CoreWindow^ window)
{
	this->renderer->InitializeDevice(Platform::Agile<Windows::UI::Core::CoreWindow>(window));
	this->renderer->Initialize(0);
	this->window->SetBounds(static_cast<float>(this->renderer->GetWidth()), static_cast<float>(this->renderer->GetHeight()));
}

#else

void GameApplication::InitializeRenderer(HWND hwnd, bool fullScreen, int width, int height)
{
	this->renderer->InitializeDevice2(hwnd, fullScreen, width, height);
	this->renderer->Initialize(0);
	this->window->SetBounds(static_cast<float>(this->renderer->GetWidth()), static_cast<float>(this->renderer->GetHeight()));
#ifdef USE_RENDER_THREAD
	thread renderThread(&GameApplication::RenderThread, this);
	renderThread.detach();
#endif
}
#endif

void GameApplication::KeyPressed(short key)
{
	if (!this->window->KeyDown(key))
	{
		if (this->window->SetKeyDown(key))
		{
			return;
		}

		switch (key)
		{
		case 'A':
			this->cameraVelocity = XMVectorAdd(this->cameraVelocity, XMVectorSet(-cameraSpeed, 0, 0, 0));			
			break;
		case 'D':
			this->cameraVelocity = XMVectorAdd(this->cameraVelocity, XMVectorSet(cameraSpeed, 0, 0, 0));
			break;
		case 'E':
			if (this->gameSimulation == NULL)
				break;
			this->gameSimulation->SendCommandPlayer(this->playerID, SimulationEngine::Command(SimulationEngine::StrafeRight, SimulationEngine::ToggleOn));
			break;
		case 'G':
			if (this->gameSimulation == NULL)
				break;
			this->gameSimulation->SendCommandPlayer(this->playerID, SimulationEngine::Command(SimulationEngine::Shoot2, SimulationEngine::ToggleOn));
			break;
		case 'H':
			BaseRendererResources::FastEverything = !BaseRendererResources::FastEverything;
			break;
		case 'Q':
			if (this->gameSimulation == NULL)
				break;
			this->gameSimulation->SendCommandPlayer(this->playerID, SimulationEngine::Command(SimulationEngine::StrafeLeft, SimulationEngine::ToggleOn));
		
			break;
		case 'S':			
			this->cameraVelocity = XMVectorAdd(this->cameraVelocity, XMVectorSet(0, 0, cameraSpeed, 0));			
			break;
		case 'W':
			this->cameraVelocity = XMVectorAdd(this->cameraVelocity, XMVectorSet(0, 0, -cameraSpeed, 0));			
			break;
		case 9: //tab key
			this->cursorActivated = !this->cursorActivated;
			this->window->SetCursorVisible(this->cursorActivated, this->hasReset);
			this->hasReset = false;
			this->gameSimulation->SetBlockAddMode(this->cursorActivated);
			break;
		case 27: //space bar
			if (this->gameSimulation == NULL)
				break;
			this->gameSimulation->SendCommandPlayer(this->playerID, SimulationEngine::Command(SimulationEngine::Jump, SimulationEngine::ToggleOn));
			break;
		case 32: //space bar
			if (this->gameSimulation == NULL)
				break;
			this->gameSimulation->SendCommandPlayer(this->playerID, SimulationEngine::Command(SimulationEngine::Jump, SimulationEngine::ToggleOn));
			break;
		}
	}
}

void GameApplication::KeyReleased(short key)
{
	if (this->window->KeyDown(key))
	{
		switch (key)
		{
		case 'A':
			{
				this->cameraVelocity = XMVectorAdd(this->cameraVelocity, XMVectorSet(cameraSpeed, 0, 0, 0));
			}
			break;
		case 'D':
			{
				this->cameraVelocity = XMVectorAdd(this->cameraVelocity, XMVectorSet(-cameraSpeed, 0, 0, 0));
			}
			break;
		case 'E':
			if (this->gameSimulation == NULL)
				break;
			this->gameSimulation->SendCommandPlayer(this->playerID, SimulationEngine::Command(SimulationEngine::StrafeRight, SimulationEngine::ToggleOff));

			break;
		case 'Q':
			if (this->gameSimulation == NULL)
				break;
			this->gameSimulation->SendCommandPlayer(this->playerID, SimulationEngine::Command(SimulationEngine::StrafeLeft, SimulationEngine::ToggleOff));

			break;
		case 'R':
			GameResources::renderWireFrame = !GameResources::renderWireFrame;
			this->renderer->ToggleWireFrame();
			break;
		case 'S':
			{
				this->cameraVelocity = XMVectorAdd(this->cameraVelocity, XMVectorSet(0, 0, -cameraSpeed, 0));
			}
			break;
		case 'W':
			{
				this->cameraVelocity = XMVectorAdd(this->cameraVelocity, XMVectorSet(0, 0, cameraSpeed, 0));
			}
			break;
		case 27: //space bar
			if (this->gameSimulation == NULL)
				break;
			this->gameSimulation->SendCommandPlayer(this->playerID, SimulationEngine::Command(SimulationEngine::Jump, SimulationEngine::ToggleOff));
			break;
		case 32: //space bar
			if (this->gameSimulation == NULL)
				break;
			this->gameSimulation->SendCommandPlayer(this->playerID, SimulationEngine::Command(SimulationEngine::Jump, SimulationEngine::ToggleOff));
			break;
		}
	}

	this->window->SetKeyUp(key);
}

void GameApplication::MouseDown(short button, float dx, float dy)
{
	if (this->cursorActivated && this->window->SetPointerDown(button, this->cursorLocation.x, this->cursorLocation.y))
	{
		return;
	}

	switch (button)
	{
	case 1:
		{
			if (this->gameSimulation == NULL)
				break;

			if (this->cursorActivated)
			{
				if (this->gameSimulation->TestCreateBlock(this->cursorWorldLocation, this->window->GetSelectedBlockType()))
				{
					this->window->RefreshUIData(this->hasReset);
					this->hasReset = false;
				}
				break; 
			}

			this->gameSimulation->SendCommandPlayer(this->playerID, SimulationEngine::Command(SimulationEngine::Shoot, SimulationEngine::ToggleOn, this->cursorLocation.x, this->cursorLocation.y));
		}
		break;
	}
}

void GameApplication::MouseUp(short button)
{

}

void GameApplication::MouseMoved(float dx, float dy)
{
	this->cursorLocation.x -= dx * GameResources::MouseSensitivity;
	this->cursorLocation.y -= dy * GameResources::MouseSensitivity;

	if (this->cursorLocation.x > 1)
		this->cursorLocation.x = 1;
	else if (this->cursorLocation.x < -1)
		this->cursorLocation.x = -1;

	if (this->cursorLocation.y > 1)
		this->cursorLocation.y = 1;
	else if (this->cursorLocation.y < -1)
		this->cursorLocation.y = -1;

	this->updateCursorWorldLocation();
	this->window->UpdateCursorLocation(this->cursorLocation.x, this->cursorLocation.y);
	if (this->gameSimulation != NULL)
	{
		this->gameSimulation->SetCursorWorldLocation(this->cursorWorldLocation);
	}

	if (this->gameSimulation == NULL || this->cursorActivated)
		return;

	this->gameSimulation->SendCommandPlayer(this->playerID, SimulationEngine::Command(SimulationEngine::Look, 0, dx, dy));
}

//Calculate the world-space location of the cursor
void GameApplication::updateCursorWorldLocation()
{
	const float fovAngleYRadians = this->renderer->GetFovAngleY();
	XMFLOAT4 camPos;
	XMStoreFloat4(&camPos, this->renderer->GetSceneGraph()->GetCameraLocation());

	const float tanAng = tanf(fovAngleYRadians / 2);
	const float nearZLen = .5f / tanAng;
	const float farZ = nearZLen + camPos.z;

	const float farHeight = tanAng * (farZ);
	const float farWidth = farHeight * this->renderer->GetAspectRatio();

	this->cursorWorldLocation = XMVectorSet(farWidth * this->cursorLocation.x + camPos.x, farHeight * this->cursorLocation.y + camPos.y, 0, 0);
}

void GameApplication::RenderThread(GameApplication* thisApplication)
{ 
	finishedRendering = false;

	while (running)
	{
		thisApplication->updateRenderer();
	}

	finishedRendering = true;
}

GameApplication::~GameApplication()
{
	if (this->gameSimulation != NULL)
	{
		delete this->gameSimulation;
	}
}