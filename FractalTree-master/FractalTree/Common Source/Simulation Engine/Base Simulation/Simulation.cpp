#include "pch.h"
#include "Simulation.h"
#ifndef _NO_GRAPHICS
#include <string>
static int objectCountIndex = -1;
#endif

namespace SimulationEngine
{
	Simulation::Simulation() :
		world(NULL)
	{

	}

	void Simulation::Initialize()
	{
	}

	void Simulation::Update()
	{
		if (this->world != NULL && !this->paused || this->oneMoreFrame)
		{
			this->world->UpdateWorld();
			this->oneMoreFrame = false;
		}
	}

	Simulation::~Simulation()
	{
		delete this->world;
	}
}