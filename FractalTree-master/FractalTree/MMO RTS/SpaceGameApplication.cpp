#include "pch.h"
#include <thread>
#include <string>

#include "SpaceGameApplication.h"
#include "SpaceGameRenderer.h"
#include "SpaceGameWindow.h"
#include "SpaceGameBindingGraph.h"
#include "UIRenderer.h"
#include "SpaceGameUDPClient.h"
#include "..\SpaceGame Common\Core\SpaceGameSimulation.h"

bool running = true;
volatile bool finishedRendering = false;
extern double performanceFrequency;

using namespace std;
using namespace SimpleUI;
using namespace RenderingEngine;
using namespace SpaceGameCore;
using namespace Network;
using namespace SpaceGameClient;
static int renderTimeIndex;
static int simulationTimeIndex;
static int networkTimeIndex;

SpaceGameApplication::SpaceGameApplication()
{
}

void SpaceGameApplication::Initialize()
{
	this->renderer = new SpaceGameRenderer();
	this->window = new SpaceGameWindow(this);
	this->renderer->SetWindow(this->window);
	this->initializeDebugStrings();
}

void SpaceGameApplication::InitializeRenderer(HWND hwnd, bool fullScreen, int width, int height)
{
	this->renderer->Initialize(hwnd, fullScreen, width, height);
	this->window->SetBounds(static_cast<float>(this->renderer->GetWidth()), static_cast<float>(this->renderer->GetHeight()));
#ifdef USE_RENDER_THREAD
	thread renderThread(&SpaceGameApplication::RenderThread, this);
	renderThread.detach();
#endif
}

void SpaceGameApplication::InitializeSimulation()
{
	if (this->simulation != nullptr)
	{
		return;
	}
	if (this->bindingGraph != nullptr)
	{
		delete this->bindingGraph;
	}
	if (this->simulation != nullptr)
	{
		delete this->simulation;
	}

	this->active = true;
	this->simulation = new SpaceGameSimulation();
	this->simulation->Initialize();
	this->window->SetSimulation(this->simulation);
	this->bindingGraph = new SpaceGameBindingGraph(this->simulation, this->renderer->GetSceneGraph(), this->renderer);

	this->playerID = 0;
	this->simulation->InitializeWorld();
	this->simulation->EnterWorld();

	this->simulation->SetLocalPlayerID(this->playerID);
	this->bindingGraph->SetClientID(this->playerID);
}

void SpaceGameApplication::InitializeSimulationNetwork(const int clientId)
{
	if (this->simulation != nullptr)
	{
		return;
	}
	this->playerID = clientId;
	if (this->bindingGraph != nullptr)
	{
		delete this->bindingGraph;
	}
	if (this->simulation != nullptr)
	{
		delete this->simulation;
	}
	this->active = true;
	this->simulation = new SpaceGameSimulation();
	this->simulation->Initialize();
	this->window->SetSimulation(this->simulation);
	this->bindingGraph = new SpaceGameBindingGraph(this->simulation, this->renderer->GetSceneGraph(), this->renderer);

	this->simulation->GetWorld()->SetActive(false);
	this->simulation->InitializeLocalPlayer(this->playerID);
	this->bindingGraph->SetClientID(this->playerID);
	this->networkClient->setSimulation(this->simulation);
	this->useNetwork = true;
}

void SpaceGameApplication::handleNetwork()
{
		
}

int SpaceGameApplication::ConnectToServer(const string& accountName, const string& password, const string& ipAddress, short port)
{
	if (this->networkClient != nullptr)
	{
		delete this->networkClient;
	}
	this->networkClient = new SpaceGameUDPClient();
	this->networkClient->InitializeNetwork();
	const int id = this->networkClient->ConnectToServer(accountName, password, ipAddress, port);
	if (id < 0)
	{
		delete this->networkClient;
		this->networkClient = nullptr;
	}

	return id;
}
void SpaceGameApplication::disconnectFromServer()
{
	this->networkClient->disconnect();
	this->quitSimulation();
}

const double maxLife = 1000;
void SpaceGameApplication::Update()
{
	if (!this->active)
		return;

	if (this->networkClient != nullptr)
	{
		_LARGE_INTEGER startTime; QueryPerformanceCounter(&startTime);

		this->networkClient->Update();

		_LARGE_INTEGER networkEndTime; QueryPerformanceCounter(&networkEndTime);
		UIRenderer::SetDebugString(networkTimeIndex, "Network Time: " + std::to_string((networkEndTime.QuadPart - startTime.QuadPart) / performanceFrequency));

		for (int i = 0; i < this->simulation->world->nextAvailableLocalID; ++i)
		{
			auto obj = this->simulation->world->GetObjectByLocalID(i);
			const auto sinceSeen = (networkEndTime.QuadPart - obj->lastUpdated.QuadPart) / performanceFrequency;
			if (sinceSeen > maxLife)
			{
				obj->Remove();
			}
		}
	}

	if (this->simulation != nullptr)
	{
		_LARGE_INTEGER startTime; QueryPerformanceCounter(&startTime);

		this->simulation->Update();

		_LARGE_INTEGER simulationEndTime; QueryPerformanceCounter(&simulationEndTime);
		UIRenderer::SetDebugString(simulationTimeIndex, "Simulation Time: " + std::to_string((simulationEndTime.QuadPart - startTime.QuadPart) / performanceFrequency));
	}

#ifndef USE_RENDER_THREAD
	this->updateRenderer();
#endif
}

void SpaceGameApplication::updateRenderer()
{
	_LARGE_INTEGER startTime;
	QueryPerformanceCounter(&startTime);

	if (this->bindingGraph != nullptr)
		this->bindingGraph->Update();
	this->renderer->Render();
	this->renderer->Present();

	_LARGE_INTEGER renderEndTime;
	QueryPerformanceCounter(&renderEndTime);

	const string renderString = to_string((renderEndTime.QuadPart - startTime.QuadPart) / performanceFrequency);
	UIRenderer::SetDebugString(renderTimeIndex, "Render Time: " + renderString);
}

void SpaceGameApplication::RenderThread(SpaceGameApplication* thisApplication)
{
	while (running)
	{
		thisApplication->updateRenderer();
	}

	finishedRendering = true;
}

void SpaceGameApplication::initializeDebugStrings()
{
	renderTimeIndex = RenderingEngine::UIRenderer::AddDebugString("Render Time: ");
	simulationTimeIndex = RenderingEngine::UIRenderer::AddDebugString("Simulation Time: ");
	networkTimeIndex = RenderingEngine::UIRenderer::AddDebugString("Network Time: ");
}

void SpaceGameApplication::sendCommand(const SimulationEngine::Command& command, const bool alwaysLocal) {
	if (this->useNetwork && !alwaysLocal)
	{
		char commandData[1024];
		const unsigned int dataLength = command.GetSerializedForm(commandData);
		this->networkClient->SendCommand(ClientActionType::SIMULATION_COMMAND, commandData, dataLength);
	}
	else
	{
		this->simulation->SendCommandPlayer(this->playerID, command);
		if (this->useNetwork)
		{
			char commandData[1024];
			const unsigned int dataLength = command.GetSerializedForm(commandData);
			this->networkClient->SendCommand(ClientActionType::SIMULATION_COMMAND, commandData, dataLength);
		}
	}
}

int SpaceGameApplication::getFocusObject()
{
	return this->simulation->GetPlayer(this->playerID)->GetFocusObject();
}

void SpaceGameApplication::quitSimulation()
{
	this->active = false;
	const auto b = this->bindingGraph;
	this->bindingGraph = nullptr;
	const auto s = this->simulation;
	this->simulation = nullptr;
	delete b;
	delete s;
}
