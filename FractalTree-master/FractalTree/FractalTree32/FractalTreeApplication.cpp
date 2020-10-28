#include "pch.h"
#include "FractalTreeApplication.h"
#include <time.h>
#include "FractalTreeRenderer.h"
#include "ActorRendererBindingGraph.h"
#include "FractalTreeWindow.h"
#include "UIRenderer.h"
#include "..\FractalTree Common\FractalTreeSimulation.h"

using namespace SimulationEngine;
using namespace RenderingEngine;
using namespace SimpleUI;
using namespace DirectX;
using namespace std;

bool running = true;
volatile bool finishedRendering = false;
extern double performanceFrequency;
int genQueueCountInd;

FractalTreeApplication::FractalTreeApplication()
{

}

void FractalTreeApplication::Initialize()
{
	this->renderer2 = new FractalTreeRenderer();
	this->window = new FractalTreeWindow(this);
	this->renderer2->SetWindow(this->window);
	this->initializeDebugStrings();
}

void FractalTreeApplication::InitializeSimulation(bool network, int worldType, int clientID)
{
	if (this->simulation != nullptr)
		return;

	if (this->bindingGraph != NULL)
	{
		delete this->bindingGraph;
	}
	if(this->simulation != NULL)
	{
		delete this->simulation;
	}

	this->simulation = new FractalTreeSimulation();
	this->simulation->Initialize();
	if (worldType == 0)
		this->simulation->generateTerrain = true;
	this->window->SetSimulation(this->simulation);
	this->bindingGraph = new ActorRendererBindingGraph(this->simulation, this->renderer2->GetSceneGraph(), this->renderer2);

	this->playerID = 0;
	switch (worldType)
	{
	case 0: //pressing enter world invokes this case.
		this->simulation->InitializeWorld();
		this->simulation->EnterWorld();
		break;
	case 1:
		this->simulation->InitializeWorld(false);
		this->simulation->EnterWorld(false);
		break;
	default:
		this->simulation->InitializeWorld(false);
		this->simulation->EnterWorld(false);
		break;
	}

	this->simulation->SetLocalPlayerID(this->playerID);
	this->bindingGraph->SetClientID(this->playerID);
}

void FractalTreeApplication::InitializeRenderer(HWND hwnd, bool fullScreen, int width, int height)
{
	this->renderer2->Initialize(hwnd, fullScreen, width, height);
	this->window->SetBounds(static_cast<float>(this->renderer2->GetWidth()), static_cast<float>(this->renderer2->GetHeight()));
	this->window->InitializeUI();
#ifdef USE_RENDER_THREAD
	thread renderThread(&FractalTreeApplication::RenderThread, this);
	renderThread.detach();
#endif
}

void FractalTreeApplication::Update()
{
	if(this->simulation != NULL)
	{
		_LARGE_INTEGER startTime; QueryPerformanceCounter(&startTime);

		this->simulation->Update();
		
		_LARGE_INTEGER simulationEndTime; QueryPerformanceCounter(&simulationEndTime);
		UIRenderer::SetDebugString(FractalTreeResources::SimulationTimeIndex, "Simulation Time: " + std::to_string((simulationEndTime.QuadPart - startTime.QuadPart) / performanceFrequency));
	}

#ifndef USE_RENDER_THREAD
	this->updateRenderer();
#endif
}

void FractalTreeApplication::updateRenderer()
{
	_LARGE_INTEGER startTime;
	QueryPerformanceCounter(&startTime);

	if(this->bindingGraph != NULL)
		this->bindingGraph->Update();
	this->renderer2->Render();
	this->renderer2->Present();

	_LARGE_INTEGER renderEndTime;
	QueryPerformanceCounter(&renderEndTime);

	const string renderString =  to_string((renderEndTime.QuadPart - startTime.QuadPart) / performanceFrequency);
	UIRenderer::SetDebugString(FractalTreeResources::RenderTimeIndex, "Render Time: " + renderString);
}


void FractalTreeApplication::sendCommand(const Command& command, bool alwaysLocal)
{
	this->simulation->SendCommandPlayer(this->playerID, command);
}

void FractalTreeApplication::initializeDebugStrings()
{
	FractalTreeResources::RenderTimeIndex = UIRenderer::AddDebugString("Render Time: ");
	FractalTreeResources::SimulationTimeIndex = UIRenderer::AddDebugString("Simulation Time: ");
	FractalTreeResources::cellGridWidthIndex = UIRenderer::AddDebugString("Cell Grid Width: 32");
	perlinAmpInd = UIRenderer::AddDebugString("Fog Density (N / M): 9");
	perlinFreqInd = UIRenderer::AddDebugString("Texture Repeat Count (V / B): 4");
	genQueueCountInd = UIRenderer::AddDebugString("Generation Queue Size (J / K): " + std::to_string(FractalTreeResources::cellGenerationCount));
	treeUpdateInd = UIRenderer::AddDebugString("Tree update/frame (7 / 8): " + std::to_string(FractalTreeResources::treeUpdatePerFrame));
	this->itCountInd = UIRenderer::AddDebugString("Constraint Iteration Count (X / Y): " + std::to_string(FractalTreeResources::constraintIterationCount));
}

void FractalTreeApplication::RenderThread(FractalTreeApplication* thisApplication)
{
	while(running)
	{
		thisApplication->updateRenderer();
	}	

	finishedRendering = true;
}

FractalTreeApplication::~FractalTreeApplication()
{
#ifndef USE_RENDER_THREAD
	delete this->renderer2;
#endif
}

